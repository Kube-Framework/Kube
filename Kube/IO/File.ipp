/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: IO File
 */

#pragma once

#include "File.hpp"

template<typename StringType>
    requires std::constructible_from<StringType, std::string_view>
inline StringType kF::IO::File::filenameWithExtension(void) const noexcept
{
    if (_path.empty())
        return StringType();
    auto index = _path.size();
    while (--index && ((_path[index] != '/') & (_path[index] != '\\')));
    index = bool(index) * (index + 1); // Only increment by 1 if not zero
    return _path.toView().substr(index);
}

template<typename StringType>
    requires std::constructible_from<StringType, std::string_view>
inline StringType kF::IO::File::filename(void) const noexcept
{
    const auto file = filenameWithExtension();
    auto index = file.size();
    while (--index) {
        if (file[index] == '.')
            return file.substr(0, index);
        else if ((file[index] == '/') | (file[index] == '\\'))
            break;
    }
    return file;
}

template<typename StringType>
    requires std::constructible_from<StringType, std::string_view>
inline StringType kF::IO::File::directoryPath(void) const noexcept
{
    if (_path.empty())
        return StringType();
    auto index = _path.size();
    while (--index && ((_path[index] != '/') & (_path[index] != '\\')));
    return _path.toView().substr(0, index);
}

template<kF::IO::Internal::ResizableContainer Container>
inline bool kF::IO::File::readAll(Container &container) noexcept
{
    using Range = decltype(std::declval<Container>().size());

    if (!isResource())
        ensureStream();
    const auto expectedSize = fileSize();
    container.resize(static_cast<Range>(expectedSize));
    const auto readSize = read(
        reinterpret_cast<std::uint8_t *>(container.data()),
        reinterpret_cast<std::uint8_t *>(container.data()) + expectedSize
    );
    return readSize == expectedSize;
}

template<kF::IO::Internal::ResizableContainer Container>
inline Container kF::IO::File::readAll(void) noexcept
{
    Container container;
    if (!readAll(container)) [[unlikely]]
        container.clear();
    return container;
}

template<kF::IO::Internal::WritableContainer Container>
inline bool kF::IO::File::writeAll(const Container &container) noexcept
{
    const auto from = reinterpret_cast<const std::uint8_t *>(&*std::begin(container));
    return write(from, from + container.size());
}