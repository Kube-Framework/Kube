/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Text processor
 */

#include <Kube/Core/Unicode.hpp>

#include <Kube/IO/File.hpp>

#include <Kube/UI/App.hpp>
#include <Kube/UI/UISystem.hpp>

#include "TextProcessor.hpp"

using namespace kF;

namespace kF::UI
{
    /** @brief Single glyph instance */
    struct alignas_quarter_cacheline Glyph
    {
        Area uv {};
        Point pos {};
        SpriteIndex spriteIndex {};
        Color color {};
        Point rotationOrigin {};
        float rotationAngle {};
        float vertical {};
    };
    static_assert_alignof_quarter_cacheline(Glyph);

    /** @brief Metrics of a single line */
    struct LineMetrics
    {
        std::uint32_t charCount {};
        Pixel spaceCount {};
        Pixel totalSize {};
        Pixel totalGlyphSize {};
        Pixel width {};
        bool elided {};
    };

    /** @brief Stores all parameters from a text computation */
    struct alignas_double_cacheline ComputeParameters
    {
        const Text *text {};
        const FontManager::GlyphIndexSet *glyphIndexSet {};
        const FontManager::GlyphsMetrics *glyphsMetrics {};
        Pixel spaceWidth {};
        Pixel ascender {};
        Pixel descender {};
        Pixel lineHeight {};
        Pixel baseline {};
        Pixel lineCount {};
        SpriteIndex spriteIndex {};
        Pixel elideSize {};

        /** @brief Query glyph metrics of an unicode character */
        [[nodiscard]] inline const FontManager::GlyphMetrics &getMetricsOf(const std::uint32_t desired) const noexcept
        {
            return FontManager::GetMetricsOf(*glyphIndexSet, *glyphsMetrics, desired);
        }
    };
    static_assert_fit_double_cacheline(ComputeParameters);

    /** @brief Compute all glyphs from a text in packed mode */
    template<auto GetX, auto GetY>
    static void ComputeGlyph(Glyph *&out, ComputeParameters &params) noexcept;

    /** @brief Compute all glyphs from a text in packed mode */
    template<auto GetX, auto GetY>
    [[nodiscard]] static UI::Size ComputeTextMetricsFromParameters(const ComputeParameters &params) noexcept;
}

template<>
UI::PrimitiveProcessorModel UI::PrimitiveProcessor::QueryModel<UI::Text>(void) noexcept
{
    return UI::PrimitiveProcessorModel {
        .computeShader = GPU::Shader(":/UI/Shaders/FilledQuad/Text.comp.spv"),
        .computeLocalGroupSize = 128,
        .instanceSize = sizeof(Glyph),
        .instanceAlignment = alignof(Glyph),
        .verticesPerInstance = 4,
        .indicesPerInstance = 6
    };
}

template<>
std::uint32_t UI::PrimitiveProcessor::GetInstanceCount<UI::Text>(
    const Text * const primitiveBegin,
    const Text * const primitiveEnd
) noexcept
{
    std::uint32_t count {};
    for (auto it = primitiveBegin; it != primitiveEnd; ++it) {
        auto from = it->str.begin();
        const auto to = it->str.end();
        while (true) {
            if (const auto unicode = Core::Unicode::GetNextChar(from, to); !unicode)
                break;
            else
                count += !std::isspace(unicode);
        }
        count += it->elide * ElideDotCount;
    }
    return count;
}

template<>
std::uint32_t UI::PrimitiveProcessor::InsertInstances<UI::Text>(
    const Text * const primitiveBegin,
    const Text * const primitiveEnd,
    std::uint8_t * const instanceBegin
) noexcept
{
    const auto &fontManager = App::Get().uiSystem().fontManager();
    auto * const begin = reinterpret_cast<Glyph *>(instanceBegin);
    auto *out = begin;
    ComputeParameters params;

    for (const Text &text : Core::IteratorRange { primitiveBegin, primitiveEnd }) {
        // Query compute parameters
        params.text = &text;
        params.glyphIndexSet = &fontManager.glyphIndexSetAt(text.fontIndex);
        params.glyphsMetrics = &fontManager.glyphsMetricsAt(text.fontIndex);
        params.spaceWidth = fontManager.spaceWidthAt(text.fontIndex);
        params.ascender = fontManager.ascenderAt(text.fontIndex);
        params.descender = fontManager.descenderAt(text.fontIndex);
        params.lineHeight = fontManager.lineHeightAt(text.fontIndex);
        params.spriteIndex = fontManager.spriteAt(text.fontIndex);
        params.elideSize = params.getMetricsOf('.').advance * ElideDotCount * text.elide;

        // Dispatch
        if (!text.vertical) [[likely]]
            ComputeGlyph<GetXAxis, GetYAxis>(out, params);
        else
            ComputeGlyph<GetYAxis, GetXAxis>(out, params);
    }
    return Core::Distance<std::uint32_t>(begin, out);
}

template<auto GetX, auto GetY>
static void UI::ComputeGlyph(Glyph *&out, ComputeParameters &params) noexcept
{
    // Some utility functions
    constexpr auto IsLastLine = [](const auto &params, const auto &offset) { return GetY(params.text->area.size) - GetY(offset) < params.lineHeight * 2.0f; };
    constexpr auto BreakLine = [IsLastLine](const auto &params, auto &offset, auto &maxLineWidth, auto &lastLine, auto &consecutiveSpaces) {
        maxLineWidth = std::max(maxLineWidth, GetX(offset));
        GetX(offset) = {};
        GetY(offset) += params.lineHeight;
        lastLine = IsLastLine(params, offset);
        consecutiveSpaces = {};
    };
    constexpr auto InsertGlyph = [](auto &out, const auto &params, auto &offset, const auto &metrics) {
        new (out) Glyph {
            .uv = metrics.uv,
            .pos = [&params, offset, &metrics] {
                auto pos = offset;
                GetX(pos) += metrics.bearing.x;
                GetY(pos) += !params.text->vertical ? params.ascender - metrics.bearing.y : -params.descender - (metrics.uv.size.height - metrics.bearing.y);
                return pos;
            }(),
            .spriteIndex = params.spriteIndex,
            .color = params.text->color,
            .rotationOrigin = {},
            .rotationAngle = params.text->rotationAngle,
            .vertical = float(params.text->vertical),
        };
        ++out;
        GetX(offset) += metrics.advance;
    };

    // Store the begin of glyph output
    const auto glyphBegin = out;
    // Last word cache
    auto lastWordGlyph = out;
    auto lastWordChar = params.text->str.begin();
    // Last elide cache
    auto lastElideGlyph = out;
    auto lastElideChar = params.text->str.begin();
    Pixel lastElideOffset {};
    // String cache
    auto from = params.text->str.begin();
    const auto to = params.text->str.end();
    // Position cache
    Point offset {};
    Pixel maxLineWidth {};
    Pixel consecutiveSpaces {};
    bool lastLine = IsLastLine(params, offset);

    // Loop through every character and produce glyphs
    while (true) {
        const auto unicode = Core::Unicode::GetNextChar(from, to);
        if (!unicode) [[unlikely]] {
            break;
        } else if (std::isspace(unicode)) {
            // Line break
            if (unicode == '\n') {
                BreakLine(params, offset, maxLineWidth, lastLine, consecutiveSpaces);
            // Regular or tab space
            } else
                consecutiveSpaces += unicode == ' ' ? 1.0f : params.text->spacesPerTab;
            // Save new head as last word begin
            lastWordGlyph = out;
            lastWordChar = from;
        } else [[likely]] {
            // Query glyph metrics
            const auto &metrics = params.getMetricsOf(unicode);
            // Compute next offset considering previous consecutive spaces
            const auto nextXOffset = GetX(offset) + consecutiveSpaces * params.spaceWidth + metrics.advance;
            // If the text can be elided, store the position of the last character that fits elide
            if ((lastLine & params.text->elide) && (nextXOffset + params.elideSize) < GetX(params.text->area.size)) {
                lastElideGlyph = out;
                lastElideChar = from;
                lastElideOffset = GetX(offset);
            }
            // The glyph is out of line width
            if (nextXOffset > GetX(params.text->area.size)) {
                // Elide the text on last line
                if (params.text->elide & lastLine) {
                    // Roll backward to insert pixel perfect dots
                    out = lastElideGlyph;
                    from = lastElideChar;
                    GetX(offset) = lastElideOffset;
                    const auto &dotMetrics = params.getMetricsOf('.');
                    for (auto dotIndex = 0u; dotIndex != ElideDotCount; ++dotIndex)
                        InsertGlyph(out, params, offset, dotMetrics);
                    break;
                // Fit the text on each line
                } else if (params.text->fit & (!params.text->elide | !lastLine)) {
                    out = lastWordGlyph;
                    from = lastWordChar;
                    BreakLine(params, offset, maxLineWidth, lastLine, consecutiveSpaces);
                    continue;
                // Break the line as we still have space for another line
                } else if (params.text->elide) {
                    BreakLine(params, offset, maxLineWidth, lastLine, consecutiveSpaces);
                }
            }
            // Apply previous consecutive stored spaces
            GetX(offset) += consecutiveSpaces * params.spaceWidth;
            consecutiveSpaces = {};
            // Insert glyph
            InsertGlyph(out, params, offset, metrics);
        }
    }
    // Break final line
    BreakLine(params, offset, maxLineWidth, lastLine, consecutiveSpaces);

    // Return if we don't have any character to display
    if (out == glyphBegin)
        return;

    // Compute text metrics
    Size metrics {};
    GetX(metrics) = maxLineWidth;
    GetY(metrics) = GetY(offset);

    // Compute anchored area from text metrics
    const auto area = Area::ApplyAnchor(params.text->area, metrics, params.text->anchor);
    const auto rotationOrigin = area.center();
    auto alignmentOffset = area.pos;

    // Positionate glyphs using parameter anchor & text alignment
    for (auto lineBegin = glyphBegin; lineBegin != out;) {
        // Find last character of line
        const auto lineIndex = int(GetY(lineBegin->pos)) / int(params.lineHeight);
        auto lineEnd = lineBegin + 1;
        while (lineEnd != out && lineIndex == int(GetY(lineEnd->pos)) / int(params.lineHeight))
            ++lineEnd;
        // Compute line width
        const auto lastLineGlyph = lineEnd - 1;
        const auto lineWidth = (GetX(lastLineGlyph->pos) + GetX(lastLineGlyph->uv.size)) - GetX(lineBegin->pos);
        // Adjust text alignment offset
        switch (params.text->textAlignment) {
        case TextAlignment::Left:
            break;
        case TextAlignment::Center:
            GetX(alignmentOffset) += (maxLineWidth - lineWidth) / 2.0f;
            break;
        case TextAlignment::Right:
            GetX(alignmentOffset) += maxLineWidth - lineWidth;
            break;
        case TextAlignment::Justify:
            kFAbort("[UI Text Processor] Justify text alignment not implemented");
            break;
        }
        // Move text by anchored area offset and text alignment offset
        for (; lineBegin != lineEnd; ++lineBegin) {
            lineBegin->pos = { std::round(lineBegin->pos.x + alignmentOffset.x), std::round(lineBegin->pos.y + alignmentOffset.y) };
            lineBegin->rotationOrigin = rotationOrigin;
        }
    }
}