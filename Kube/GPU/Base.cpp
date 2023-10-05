/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: GPU Base
 */

#include "Base.hpp"

using namespace kF;

std::string_view GPU::ErrorMessage(const VkResult res) noexcept
{
    switch (res)
    {
    case VK_NOT_READY:
        return "NOT_READY";
    case VK_TIMEOUT:
        return "TIMEOUT";
    case VK_EVENT_SET:
        return "EVENT_SET";
    case VK_EVENT_RESET:
        return "EVENT_RESET";
    case VK_INCOMPLETE:
        return "INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED:
        return "ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST:
        return "ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED:
        return "ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT:
        return "ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS:
        return "ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_SURFACE_LOST_KHR:
        return "ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return "ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR:
        return "SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR:
        return "ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT:
        return "ERROR_VALIDATION_FAILED_EXT";
    case VK_ERROR_INVALID_SHADER_NV:
        return "ERROR_INVALID_SHADER_NV";
    default:
        return "UNKNOWN_ERROR";
    }
}

std::string_view kF::GPU::QueueTypeName(const QueueType type) noexcept
{
    switch (type) {
    case QueueType::Graphics:
        return "Graphics";
    case QueueType::Compute:
        return "Compute";
    case QueueType::Transfer:
        return "Transfer";
    case QueueType::Present:
        return "Present";
    default:
        return "UnknownQueueType";
    }
}

std::string_view GPU::PresentModeName(const PresentMode type) noexcept
{
    switch (type) {
    case PresentMode::ImmediateKhr:
        return "ImmediateKhr";
    case PresentMode::MailboxKhr:
        return "MailboxKhr";
    case PresentMode::FifoKhr:
        return "FifoKhr";
    case PresentMode::FifoRelaxedKhr:
        return "FifoRelaxedKhr";
    default:
        return "UnknownPresentMode";
    }
}

std::string_view GPU::FormatName(const Format type) noexcept
{
    switch (type) {
    case Format::Undefined:
        return "Undefined";
    case Format::R4G4_UNORM_PACK8:
        return "R4G4_UNORM_PACK8";
    case Format::R4G4B4A4_UNORM_PACK16:
        return "R4G4B4A4_UNORM_PACK16";
    case Format::B4G4R4A4_UNORM_PACK16:
        return "B4G4R4A4_UNORM_PACK16";
    case Format::R5G6B5_UNORM_PACK16:
        return "R5G6B5_UNORM_PACK16";
    case Format::B5G6R5_UNORM_PACK16:
        return "B5G6R5_UNORM_PACK16";
    case Format::R5G5B5A1_UNORM_PACK16:
        return "R5G5B5A1_UNORM_PACK16";
    case Format::B5G5R5A1_UNORM_PACK16:
        return "B5G5R5A1_UNORM_PACK16";
    case Format::A1R5G5B5_UNORM_PACK16:
        return "A1R5G5B5_UNORM_PACK16";
    case Format::R8_UNORM:
        return "R8_UNORM";
    case Format::R8_SNORM:
        return "R8_SNORM";
    case Format::R8_USCALED:
        return "R8_USCALED";
    case Format::R8_SSCALED:
        return "R8_SSCALED";
    case Format::R8_UINT:
        return "R8_UINT";
    case Format::R8_SINT:
        return "R8_SINT";
    case Format::R8_SRGB:
        return "R8_SRGB";
    case Format::R8G8_UNORM:
        return "R8G8_UNORM";
    case Format::R8G8_SNORM:
        return "R8G8_SNORM";
    case Format::R8G8_USCALED:
        return "R8G8_USCALED";
    case Format::R8G8_SSCALED:
        return "R8G8_SSCALED";
    case Format::R8G8_UINT:
        return "R8G8_UINT";
    case Format::R8G8_SINT:
        return "R8G8_SINT";
    case Format::R8G8_SRGB:
        return "R8G8_SRGB";
    case Format::R8G8B8_UNORM:
        return "R8G8B8_UNORM";
    case Format::R8G8B8_SNORM:
        return "R8G8B8_SNORM";
    case Format::R8G8B8_USCALED:
        return "R8G8B8_USCALED";
    case Format::R8G8B8_SSCALED:
        return "R8G8B8_SSCALED";
    case Format::R8G8B8_UINT:
        return "R8G8B8_UINT";
    case Format::R8G8B8_SINT:
        return "R8G8B8_SINT";
    case Format::R8G8B8_SRGB:
        return "R8G8B8_SRGB";
    case Format::B8G8R8_UNORM:
        return "B8G8R8_UNORM";
    case Format::B8G8R8_SNORM:
        return "B8G8R8_SNORM";
    case Format::B8G8R8_USCALED:
        return "B8G8R8_USCALED";
    case Format::B8G8R8_SSCALED:
        return "B8G8R8_SSCALED";
    case Format::B8G8R8_UINT:
        return "B8G8R8_UINT";
    case Format::B8G8R8_SINT:
        return "B8G8R8_SINT";
    case Format::B8G8R8_SRGB:
        return "B8G8R8_SRGB";
    case Format::R8G8B8A8_UNORM:
        return "R8G8B8A8_UNORM";
    case Format::R8G8B8A8_SNORM:
        return "R8G8B8A8_SNORM";
    case Format::R8G8B8A8_USCALED:
        return "R8G8B8A8_USCALED";
    case Format::R8G8B8A8_SSCALED:
        return "R8G8B8A8_SSCALED";
    case Format::R8G8B8A8_UINT:
        return "R8G8B8A8_UINT";
    case Format::R8G8B8A8_SINT:
        return "R8G8B8A8_SINT";
    case Format::R8G8B8A8_SRGB:
        return "R8G8B8A8_SRGB";
    case Format::B8G8R8A8_UNORM:
        return "B8G8R8A8_UNORM";
    case Format::B8G8R8A8_SNORM:
        return "B8G8R8A8_SNORM";
    case Format::B8G8R8A8_USCALED:
        return "B8G8R8A8_USCALED";
    case Format::B8G8R8A8_SSCALED:
        return "B8G8R8A8_SSCALED";
    case Format::B8G8R8A8_UINT:
        return "B8G8R8A8_UINT";
    case Format::B8G8R8A8_SINT:
        return "B8G8R8A8_SINT";
    case Format::B8G8R8A8_SRGB:
        return "B8G8R8A8_SRGB";
    case Format::A8B8G8R8_UNORM_PACK32:
        return "A8B8G8R8_UNORM_PACK32";
    case Format::A8B8G8R8_SNORM_PACK32:
        return "A8B8G8R8_SNORM_PACK32";
    case Format::A8B8G8R8_USCALED_PACK32:
        return "A8B8G8R8_USCALED_PACK32";
    case Format::A8B8G8R8_SSCALED_PACK32:
        return "A8B8G8R8_SSCALED_PACK32";
    case Format::A8B8G8R8_UINT_PACK32:
        return "A8B8G8R8_UINT_PACK32";
    case Format::A8B8G8R8_SINT_PACK32:
        return "A8B8G8R8_SINT_PACK32";
    case Format::A8B8G8R8_SRGB_PACK32:
        return "A8B8G8R8_SRGB_PACK32";
    case Format::A2R10G10B10_UNORM_PACK32:
        return "A2R10G10B10_UNORM_PACK32";
    case Format::A2R10G10B10_SNORM_PACK32:
        return "A2R10G10B10_SNORM_PACK32";
    case Format::A2R10G10B10_USCALED_PACK32:
        return "A2R10G10B10_USCALED_PACK32";
    case Format::A2R10G10B10_SSCALED_PACK32:
        return "A2R10G10B10_SSCALED_PACK32";
    case Format::A2R10G10B10_UINT_PACK32:
        return "A2R10G10B10_UINT_PACK32";
    case Format::A2R10G10B10_SINT_PACK32:
        return "A2R10G10B10_SINT_PACK32";
    case Format::A2B10G10R10_UNORM_PACK32:
        return "A2B10G10R10_UNORM_PACK32";
    case Format::A2B10G10R10_SNORM_PACK32:
        return "A2B10G10R10_SNORM_PACK32";
    case Format::A2B10G10R10_USCALED_PACK32:
        return "A2B10G10R10_USCALED_PACK32";
    case Format::A2B10G10R10_SSCALED_PACK32:
        return "A2B10G10R10_SSCALED_PACK32";
    case Format::A2B10G10R10_UINT_PACK32:
        return "A2B10G10R10_UINT_PACK32";
    case Format::A2B10G10R10_SINT_PACK32:
        return "A2B10G10R10_SINT_PACK32";
    case Format::R16_UNORM:
        return "R16_UNORM";
    case Format::R16_SNORM:
        return "R16_SNORM";
    case Format::R16_USCALED:
        return "R16_USCALED";
    case Format::R16_SSCALED:
        return "R16_SSCALED";
    case Format::R16_UINT:
        return "R16_UINT";
    case Format::R16_SINT:
        return "R16_SINT";
    case Format::R16_SFLOAT:
        return "R16_SFLOAT";
    case Format::R16G16_UNORM:
        return "R16G16_UNORM";
    case Format::R16G16_SNORM:
        return "R16G16_SNORM";
    case Format::R16G16_USCALED:
        return "R16G16_USCALED";
    case Format::R16G16_SSCALED:
        return "R16G16_SSCALED";
    case Format::R16G16_UINT:
        return "R16G16_UINT";
    case Format::R16G16_SINT:
        return "R16G16_SINT";
    case Format::R16G16_SFLOAT:
        return "R16G16_SFLOAT";
    case Format::R16G16B16_UNORM:
        return "R16G16B16_UNORM";
    case Format::R16G16B16_SNORM:
        return "R16G16B16_SNORM";
    case Format::R16G16B16_USCALED:
        return "R16G16B16_USCALED";
    case Format::R16G16B16_SSCALED:
        return "R16G16B16_SSCALED";
    case Format::R16G16B16_UINT:
        return "R16G16B16_UINT";
    case Format::R16G16B16_SINT:
        return "R16G16B16_SINT";
    case Format::R16G16B16_SFLOAT:
        return "R16G16B16_SFLOAT";
    case Format::R16G16B16A16_UNORM:
        return "R16G16B16A16_UNORM";
    case Format::R16G16B16A16_SNORM:
        return "R16G16B16A16_SNORM";
    case Format::R16G16B16A16_USCALED:
        return "R16G16B16A16_USCALED";
    case Format::R16G16B16A16_SSCALED:
        return "R16G16B16A16_SSCALED";
    case Format::R16G16B16A16_UINT:
        return "R16G16B16A16_UINT";
    case Format::R16G16B16A16_SINT:
        return "R16G16B16A16_SINT";
    case Format::R16G16B16A16_SFLOAT:
        return "R16G16B16A16_SFLOAT";
    case Format::R32_UINT:
        return "R32_UINT";
    case Format::R32_SINT:
        return "R32_SINT";
    case Format::R32_SFLOAT:
        return "R32_SFLOAT";
    case Format::R32G32_UINT:
        return "R32G32_UINT";
    case Format::R32G32_SINT:
        return "R32G32_SINT";
    case Format::R32G32_SFLOAT:
        return "R32G32_SFLOAT";
    case Format::R32G32B32_UINT:
        return "R32G32B32_UINT";
    case Format::R32G32B32_SINT:
        return "R32G32B32_SINT";
    case Format::R32G32B32_SFLOAT:
        return "R32G32B32_SFLOAT";
    case Format::R32G32B32A32_UINT:
        return "R32G32B32A32_UINT";
    case Format::R32G32B32A32_SINT:
        return "R32G32B32A32_SINT";
    case Format::R32G32B32A32_SFLOAT:
        return "R32G32B32A32_SFLOAT";
    case Format::R64_UINT:
        return "R64_UINT";
    case Format::R64_SINT:
        return "R64_SINT";
    case Format::R64_SFLOAT:
        return "R64_SFLOAT";
    case Format::R64G64_UINT:
        return "R64G64_UINT";
    case Format::R64G64_SINT:
        return "R64G64_SINT";
    case Format::R64G64_SFLOAT:
        return "R64G64_SFLOAT";
    case Format::R64G64B64_UINT:
        return "R64G64B64_UINT";
    case Format::R64G64B64_SINT:
        return "R64G64B64_SINT";
    case Format::R64G64B64_SFLOAT:
        return "R64G64B64_SFLOAT";
    case Format::R64G64B64A64_UINT:
        return "R64G64B64A64_UINT";
    case Format::R64G64B64A64_SINT:
        return "R64G64B64A64_SINT";
    case Format::R64G64B64A64_SFLOAT:
        return "R64G64B64A64_SFLOAT";
    case Format::B10G11R11_UFLOAT_PACK32:
        return "B10G11R11_UFLOAT_PACK32";
    case Format::E5B9G9R9_UFLOAT_PACK32:
        return "E5B9G9R9_UFLOAT_PACK32";
    case Format::D16_UNORM:
        return "D16_UNORM";
    case Format::X8_D24_UNORM_PACK32:
        return "X8_D24_UNORM_PACK32";
    case Format::D32_SFLOAT:
        return "D32_SFLOAT";
    case Format::S8_UINT:
        return "S8_UINT";
    case Format::D16_UNORM_S8_UINT:
        return "D16_UNORM_S8_UINT";
    case Format::D24_UNORM_S8_UINT:
        return "D24_UNORM_S8_UINT";
    case Format::D32_SFLOAT_S8_UINT:
        return "D32_SFLOAT_S8_UINT";
    default:
        return "UnknownFormat";
    }
}