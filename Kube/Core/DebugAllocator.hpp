/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Debug allocator
 */

#pragma once

#include <mutex>

#include "IAllocator.hpp"
#include "Log.hpp"
#include "Vector.hpp"
#include "String.hpp"
#include "StringUtils.hpp"

namespace kF::Core
{
    template<typename Base>
    class DebugAllocator;
}

template<typename Base>
class kF::Core::DebugAllocator : public Base
{
public:
    /** @brief Virtual destructor */
    ~DebugAllocator(void) noexcept override = default;

    /** @brief Name constructor */
    DebugAllocator(const std::string_view &name = "DebugAllocator") noexcept : _name(name) {}


    /** @brief Allocate function implementation */
    [[nodiscard]] void *allocate(const std::size_t size, const std::size_t alignment) noexcept override
    {
        auto data = Base::allocate(size, alignment);
        auto l = std::unique_lock(_m);
        kFInfo(_name, "::allocate(", size, ", ", alignment, ") = ", data);
        _allocs.push(data);
        _data.push(std::make_pair(size, alignment));
        return data;
    }

    /** @brief Deallocate function implementation */
    void deallocate(void * const data, const std::size_t size, const std::size_t alignment) noexcept override
    {
        {
            auto l = std::unique_lock(_m);
            auto it = _allocs.find(data);
            auto idx = std::distance(_allocs.begin(), it);
            if (it == _allocs.end()) {
                kFError("/!\\ ", _name, "::deallocate(", data, ", ", size, ", ", alignment, "): Unknown pointer");
                return;
            } else if (auto pair = _data[idx]; pair.first != size || pair.second != alignment) {
                kFError("/!\\ ", _name, "::deallocate(", data, ", ", size, ", ", alignment, "): Pointer was allocated with size ",
                        pair.first, " and alignment ", pair.second);
            } else {
                kFInfo(_name, "::deallocate(", data, ", ", size, ", ", alignment, ")");
            }
            _allocs.erase(it);
            _data.erase(_data.begin() + idx);
        }
        Base::deallocate(data, size, alignment);
    }

private:
    std::mutex _m {};
    Core::String _name {};
    Core::Vector<void *> _allocs {};
    Core::Vector<std::pair<std::size_t, std::size_t>> _data {};
};