/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: IO File
 */

#pragma once

#include <Kube/Core/Hash.hpp>
#include <Kube/Core/SmallString.hpp>

#include "Base.hpp"

#include <fstream>

namespace kF::IO
{
    class File;

    namespace Internal
    {
        template<typename Container>
        concept ResizableContainer =
            (sizeof(std::remove_cvref_t<decltype(*std::begin(std::declval<Container>()))>) == sizeof(std::byte))
            && requires(Container &container) {
                container.data();
                container.size();
                container.resize(std::declval<std::size_t>());
            };

        template<typename Container>
        concept WritableContainer =
            (sizeof(std::remove_cvref_t<decltype(*std::begin(std::declval<Container>()))>) == sizeof(std::byte))
            && requires(Container &container) {
                std::begin(container);
                std::end(container);
            };
    }
}

class kF::IO::File
{
public:
    /** @brief Open modes */
    enum class Mode
    {
        None                = 0b0000000,
        Read                = 0b0000001,
        Write               = 0b0000010,
        ReadAndWrite        = 0b0000011,
        ReadBinary          = 0b0000101,
        WriteBinary         = 0b0000110,
        ReadAndWriteBinary  = 0b0000111
    };

    /** @brief Check if a mode is binary */
    [[nodiscard]] static constexpr bool IsBinary(const Mode mode) noexcept
        { return Core::ToUnderlying(mode) & Core::ToUnderlying(Core::RemoveFlags(Mode::ReadAndWriteBinary, Mode::Read, Mode::Write)); }


    /** @brief Destructor */
    ~File(void) noexcept = default;

    /** @brief Default constructor */
    File(void) noexcept = default;

    /** @brief Deleted copy constructor */
    File(const File &path) noexcept = delete;

    /** @brief Move constructor */
    File(File &&path) noexcept = default;

    /** @brief Set file of given 'path' */
    File(const std::string_view &path, const Mode mode = Mode::None) noexcept;

    /** @brief Deleted copy assignment */
    File &operator=(const File &path) noexcept = delete;

    /** @brief Move assignment */
    File &operator=(File &&path) noexcept = default;


    /** @brief Get file path */
    template<typename StringType = std::string_view>
        requires std::constructible_from<StringType, std::string_view>
    [[nodiscard]] StringType path(void) const noexcept { return _path; }

    /** @brief Get file name with its extension */
    template<typename StringType = std::string_view>
        requires std::constructible_from<StringType, std::string_view>
    [[nodiscard]] StringType filenameWithExtension(void) const noexcept;

    /** @brief Get file name */
    template<typename StringType = std::string_view>
        requires std::constructible_from<StringType, std::string_view>
    [[nodiscard]] StringType filename(void) const noexcept;

    /** @brief Get directory path */
    template<typename StringType = std::string_view>
        requires std::constructible_from<StringType, std::string_view>
    [[nodiscard]] StringType directoryPath(void) const noexcept;


    /** @brief Check if the file is a resource file (embedded into the executable) */
    [[nodiscard]] inline bool isResource(void) const noexcept { return _environmentHash; }

    /** @brief Get environment name */
    [[nodiscard]] inline Core::HashedName environmentHash(void) const noexcept { return _environmentHash; }

    /** @brief Get environment name */
    [[nodiscard]] inline std::string_view environment(void) const noexcept
        { return _path.toView().substr(EnvironmentBeginIndex, _environmentTo - EnvironmentBeginIndex); }


    /** @brief Check if resource exists */
    [[nodiscard]] bool resourceExists(void) const noexcept;

    /** @brief Get resource path */
    [[nodiscard]] inline std::string_view resourcePath(void) const noexcept
        { return _path.toView().substr(_environmentTo + 1); }

    /** @brief Query a resource */
    [[nodiscard]] ResourceView queryResource(void) const noexcept;


    /** @brief Check if the file exists */
    [[nodiscard]] bool exists(void) const noexcept;

    /** @brief Get the file size */
    [[nodiscard]] std::size_t fileSize(void) const noexcept;


    /** @brief Get current offset */
    [[nodiscard]] std::size_t offset(void) const noexcept { return _offset; }

    /** @brief Set or write offset */
    void setOffset(const std::size_t offset) noexcept;


    /** @brief Read data and store it into range (use internal offset) */
    [[nodiscard]] inline std::size_t read(std::uint8_t * const from, std::uint8_t * const to) noexcept
        { return read(from, to, _offset); }

    /** @brief Read data and store it into range
     *  @param offset Offset in byte from where to start reading the file */
    [[nodiscard]] std::size_t read(std::uint8_t * const from, std::uint8_t * const to, const std::size_t offset) noexcept;

    /** @brief Read all file data and store it into custom container */
    template<kF::IO::Internal::ResizableContainer Container>
    [[nodiscard]] bool readAll(Container &container) noexcept;

    /** @brief Read all file data and store it into custom container */
    template<kF::IO::Internal::ResizableContainer Container>
    [[nodiscard]] Container readAll(void) noexcept;


    /** @brief Write data range to file (use internal offset)
     *  @note Resource files are read-only */
    [[nodiscard]] inline bool write(const std::uint8_t * const from, const std::uint8_t * const to) noexcept
        { return write(from, to, _offset); }

    /** @brief Write data range to file
     *  @param offset Offset in byte from where to start reading the file
     *  @note Resource files are read-only */
    [[nodiscard]] bool write(const std::uint8_t * const from, const std::uint8_t * const to, const std::size_t offset) noexcept;

    /** @brief Read all file data and store it into custom container */
    template<kF::IO::Internal::WritableContainer Container>
    [[nodiscard]] bool writeAll(const Container &container) noexcept;


    /** @brief Copy file to another location */
    bool copy(const std::string_view &destination) const noexcept;

    /** @brief Move file
     *  @warning This function do not changes the file instance path */
    bool move(const std::string_view &destination) const noexcept;

    /** @brief Remove file */
    bool remove(void) const noexcept;


private:
    /** @brief Ensure that this instance has an allocated stream */
    void ensureStream(void) noexcept;

    Core::SmallString<IOAllocator> _path {};
    Core::HashedName _environmentHash {};
    std::uint32_t _environmentTo {};
    Mode _mode {};
    std::size_t _offset {};
    std::size_t _fileSize {};
    std::fstream _stream {};
};

#include "File.ipp"