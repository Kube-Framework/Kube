/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: IO File
 */

#include "File.hpp"
#include "ResourceManager.hpp"

#include <Kube/Core/Abort.hpp>
#include <Kube/Core/Assert.hpp>

#include <filesystem>

using namespace kF;

IO::File::File(const std::string_view &path, const Mode mode) noexcept
    : _path(path), _mode(mode)
{
    if (path.starts_with(ResourcePrefix)) {
        const auto to = static_cast<std::uint32_t>(path.find('/', EnvironmentBeginIndex));
        _environmentTo = to;
        _environmentHash = Core::Hash(path.substr(EnvironmentBeginIndex, to - EnvironmentBeginIndex));
    }
}

bool IO::File::resourceExists(void) const noexcept
{
    return ResourceManager::Get().resourceExists(
        _environmentHash,
        resourcePath()
    );
}

IO::ResourceView IO::File::queryResource(void) const noexcept
{
    return ResourceManager::Get().queryResource(
        _environmentHash,
        resourcePath()
    );
}

bool IO::File::exists(void) const noexcept
{
    if (isResource())
        return resourceExists();
    else
        return std::filesystem::exists(std::filesystem::path(_path.toView()));
}

std::size_t IO::File::fileSize(void) const noexcept
{
    if (isResource())
        return queryResource().size();
    else if (_stream.is_open())
        return _fileSize;
    else
        return std::filesystem::file_size(_path.toView());
}

void IO::File::setOffset(const std::size_t offset) noexcept
{
    if (_offset != offset) {
        _offset = offset;
        if (!isResource())
            _stream.seekg(static_cast<std::streamoff>(offset));
    }
}

std::size_t IO::File::read(std::uint8_t * const from, std::uint8_t * const to, const std::size_t offset) noexcept
{
    kFEnsure(Core::HasFlags(_mode, Mode::Read), "IO::File::write: File not opened for reading");

    constexpr auto GetReadSize = [](const std::size_t offset, const std::size_t desired, const std::size_t size) {
        if (offset < size) [[likely]]
            return std::min(size - offset, desired);
        else [[unlikely]]
            return static_cast<std::size_t>(0ul);
    };

    const auto count = static_cast<std::size_t>(std::distance(from, to));

    if (isResource()) {
        const auto range = queryResource();
        const auto readCount = GetReadSize(offset, count, range.size());
        if (readCount) [[likely]] {
            const auto begin = range.begin() + offset;
            std::copy(begin, begin + readCount, from);
            _offset += readCount;
        }
        return readCount;
    } else {
        ensureStream();
        auto readCount = GetReadSize(offset, count, _fileSize);
        if (readCount) [[likely]] {
            setOffset(offset);
            if (_stream.good()) {
                _stream.read(reinterpret_cast<char *>(from), static_cast<std::streamoff>(readCount));
                _offset += readCount;
            } else
                return 0u;
        }
        return readCount;
    }
}

bool IO::File::write(const std::uint8_t * const from, const std::uint8_t * const to, const std::size_t offset) noexcept
{
    kFEnsure(!isResource(), "IO::File::write: Cannot write into resource file");
    kFEnsure(Core::HasFlags(_mode, Mode::Write), "IO::File::write: File not opened for writing");
    ensureStream();
    setOffset(offset);
    const auto writeCount = std::distance(from, to);
    _stream.write(reinterpret_cast<const char *>(from), writeCount);
    _offset += std::size_t(writeCount);
    return _stream.good();
}

bool kF::IO::File::copy(const std::string_view &destination) const noexcept
{
    if (!exists())
        return false;
    else if (isResource()) {
        File copy(destination, Mode::WriteBinary);
        return copy.writeAll(queryResource());
    } else
        return std::filesystem::copy_file(std::filesystem::path(_path.toView()), std::filesystem::path(destination));
}

bool kF::IO::File::move(const std::string_view &destination) const noexcept
{
    if (isResource() || !exists()) {
        return false;
    } else {
        std::error_code code {};
        std::filesystem::rename(std::filesystem::path(_path.toView()), std::filesystem::path(destination), code);
        return !code;
    }
}

bool kF::IO::File::remove(void) const noexcept
{
    if (isResource() || !exists())
        return false;
    else
        return std::filesystem::remove(std::filesystem::path(_path.toView()));
}

void kF::IO::File::ensureStream(void) noexcept
{
    if (!_stream.is_open()) {
        auto mode
            = (Core::HasFlags(_mode, Mode::Write) ? std::ios::out : std::ios::openmode())
            | (Core::HasFlags(_mode, Mode::Read) ? std::ios::in : std::ios::openmode())
            | (IsBinary(_mode) ? std::ios::binary : std::ios::openmode());
        _stream.open(std::filesystem::path(_path.toView()), mode);
        kFEnsure(_stream.good(), "UI::File::ensureStream: Stream opened with invalid file path '", _path, '\'');
        _fileSize = std::filesystem::file_size(_path.toView());
    }
}