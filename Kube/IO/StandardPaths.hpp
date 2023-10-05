/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: IO StandardPaths
 */

#pragma once

#include <Kube/Core/Utils.hpp>

namespace kF::IO
{
    /** @brief Default guessed path size */
    constexpr std::size_t DefaultPathSize = 260;


    /** @brief Get executable path */
    template<typename Type>
    [[nodiscard]] Type GetExecutablePath(void) noexcept;

    /** @brief Get executable directory */
    template<typename Type>
    [[nodiscard]] Type GetExecutableDirectory(void) noexcept;

    /** @brief Get config path*/
    template<typename Type>
	[[nodiscard]] Type GetConfigPath(void) noexcept;

    /** @brief Get data path*/
    template<typename Type>
	[[nodiscard]] Type GetDataPath(void) noexcept;

    /** @brief Get state path*/
    template<typename Type>
	[[nodiscard]] Type GetStatePath(void) noexcept;

    /** @brief Get cache path*/
    template<typename Type>
	[[nodiscard]] Type GetCachePath(void) noexcept;

    /** @brief Get documents path*/
    template<typename Type>
	[[nodiscard]] Type GetDocumentsPath(void) noexcept;

    /** @brief Get desktop path*/
    template<typename Type>
	[[nodiscard]] Type GetDesktopPath(void) noexcept;

    /** @brief Get pictures path*/
    template<typename Type>
	[[nodiscard]] Type GetPicturesPath(void) noexcept;

    /** @brief Get music path*/
    template<typename Type>
	[[nodiscard]] Type GetMusicPath(void) noexcept;

    /** @brief Get video path*/
    template<typename Type>
	[[nodiscard]] Type GetVideoPath(void) noexcept;

    /** @brief Get download path*/
    template<typename Type>
	[[nodiscard]] Type GetDownloadPath(void) noexcept;

    /** @brief Get save path*/
    template<typename Type>
	[[nodiscard]] Type GetSavePath(void) noexcept;
}

#include "StandardPaths.ipp"