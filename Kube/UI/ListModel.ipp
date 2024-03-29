/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ListModel
 */

#include "ListModel.hpp"

template<typename DataType>
inline kF::UI::ListModelEvent::ListModelEvent(const DataType &data_) noexcept
{
    static_assert(std::is_same_v<DataType, Insert>
        || std::is_same_v<DataType, Erase>
        || std::is_same_v<DataType, Update>
        || std::is_same_v<DataType, Resize>
        || std::is_same_v<DataType, Move>,
            "UI::ListModelEvent: Invalid DataType");

    if constexpr (std::is_same_v<DataType, Insert>) {
        type = Type::Insert;
        new (&insert) ListModelEvent::Insert { data_ };
    } else if constexpr (std::is_same_v<DataType, Erase>) {
        type = Type::Erase;
        new (&erase) ListModelEvent::Erase { data_ };
    } else if constexpr (std::is_same_v<DataType, Update>) {
        type = Type::Update;
        new (&update) ListModelEvent::Update { data_ };
    } else if constexpr (std::is_same_v<DataType, Resize>) {
        type = Type::Resize;
        new (&resize) ListModelEvent::Resize { data_ };
    } else if constexpr (std::is_same_v<DataType, Move>) {
        type = Type::Move;
        new (&move) ListModelEvent::Move { data_ };
    }
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::UI::ListModel<Container, Allocator>::invalidate(const ConstIterator from, const ConstIterator to) noexcept
{
    const auto begin_ = cbegin();
    invalidate(Core::Distance<Range>(begin_, from), Core::Distance<Range>(begin_, to));
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::UI::ListModel<Container, Allocator>::invalidate(const Range from, const Range to) noexcept
{
    _dispatcher.dispatch(ListModelEvent::Update {
        .from = from,
        .to = to
    });
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
template<typename ...Args>
inline kF::UI::ListModel<Container, Allocator>::Type &kF::UI::ListModel<Container, Allocator>::push(Args &&...args) noexcept
{
    const auto index = _container.size();
    auto &ref = _container.push(std::forward<Args>(args)...);
    _dispatcher.dispatch(ListModelEvent::Insert {
        .from = index,
        .to = index + 1
    });
    return ref;
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::UI::ListModel<Container, Allocator>::pop(void) noexcept
{
    _container.pop();
    const auto index = _container.size();
    _dispatcher.dispatch(ListModelEvent::Erase {
        .from = index,
        .to = index + 1
    });
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::UI::ListModel<Container, Allocator>::Iterator kF::UI::ListModel<Container, Allocator>::insertDefault(const Iterator pos, const Range count) noexcept
{
    const auto index = Core::Distance<Range>(begin(), pos);
    const auto it = _container.insertDefault(pos, count);
    _dispatcher.dispatch(ListModelEvent::Insert {
        .from = index,
        .to = index + count
    });
    return it;
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::UI::ListModel<Container, Allocator>::Iterator kF::UI::ListModel<Container, Allocator>::insertFill(const Iterator pos, const Range count, const Type &value) noexcept
{
    const auto index = Core::Distance<Range>(begin(), pos);
    const auto it = _container.insertFill(pos, count, value);
    _dispatcher.dispatch(ListModelEvent::Insert {
        .from = index,
        .to = index + count
    });
    return it;
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
template<std::input_iterator InputIterator>
inline kF::UI::ListModel<Container, Allocator>::Iterator kF::UI::ListModel<Container, Allocator>::insert(const Iterator pos, const InputIterator from, const InputIterator to) noexcept
{
    const auto index = Core::Distance<Range>(begin(), pos);
    const auto count = Core::Distance<Range>(from, to);
    const auto it = _container.insert(pos, from, to);
    _dispatcher.dispatch(ListModelEvent::Insert {
        .from = index,
        .to = index + count
    });
    return it;
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
template<std::input_iterator InputIterator, typename Map>
inline kF::UI::ListModel<Container, Allocator>::Iterator kF::UI::ListModel<Container, Allocator>::insert(const Iterator pos, const InputIterator from, const InputIterator to, Map &&map) noexcept
{
    const auto index = Core::Distance<Range>(begin(), pos);
    const auto count = Core::Distance<Range>(from, to);
    const auto it = _container.insert(pos, from, to, std::forward<Map>(map));
    _dispatcher.dispatch(ListModelEvent::Insert {
        .from = index,
        .to = index + count
    });
    return it;
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
template<typename InsertFunc>
inline kF::UI::ListModel<Container, Allocator>::Iterator kF::UI::ListModel<Container, Allocator>::insertCustom(const Iterator pos, const Range count, InsertFunc &&insertFunc) noexcept
{
    const auto index = Core::Distance<Range>(begin(), pos);
    const auto it = _container.insertCustom(pos, count, std::forward<InsertFunc>(insertFunc));
    _dispatcher.dispatch(ListModelEvent::Insert {
        .from = index,
        .to = index + count
    });
    return it;
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::UI::ListModel<Container, Allocator>::Iterator kF::UI::ListModel<Container, Allocator>::erase(Iterator from, Iterator to) noexcept
{
    const auto index = Core::Distance<Range>(begin(), from);
    const auto count = Core::Distance<Range>(from, to);
    const auto it = _container.erase(from, to);
    _dispatcher.dispatch(ListModelEvent::Erase {
        .from = index,
        .to = index + count
    });
    return it;
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::UI::ListModel<Container, Allocator>::resize(const Range count) noexcept
    requires std::constructible_from<kF::UI::ListModel<Container, Allocator>::Type>
{
    _container.resize(count);
    _dispatcher.dispatch(ListModelEvent::Resize {
        .count = count
    });
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::UI::ListModel<Container, Allocator>::resize(const Range count, const Type &value) noexcept
    requires std::copy_constructible<kF::UI::ListModel<Container, Allocator>::Type>
{
    _container.resize(count, value);
    _dispatcher.dispatch(ListModelEvent::Resize {
        .count = count
    });
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
template<typename Initializer>
void kF::UI::ListModel<Container, Allocator>::resize(const Range count, Initializer &&initializer) noexcept
{
    _container.resize(count, std::forward<Initializer>(initializer));
    _dispatcher.dispatch(ListModelEvent::Resize {
        .count = count
    });
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
template<std::input_iterator InputIterator>
inline void kF::UI::ListModel<Container, Allocator>::resize(const InputIterator from, const InputIterator to) noexcept
{
    _container.resize(from, to);
    _dispatcher.dispatch(ListModelEvent::Resize {
        .count = Core::Distance<Range>(from, to)
    });
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
template<std::input_iterator InputIterator, typename Map>
inline void kF::UI::ListModel<Container, Allocator>::resize(const InputIterator from, const InputIterator to, Map &&map) noexcept
{
    _container.resize(from, to, std::forward<Map>(map));
    _dispatcher.dispatch(ListModelEvent::Resize {
        .count = Core::Distance<Range>(from, to)
    });
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::UI::ListModel<Container, Allocator>::clear(void) noexcept
{
    const auto count = _container.size();
    _container.clear();
    if (count) [[likely]] {
        _dispatcher.dispatch(ListModelEvent::Erase {
            .from = 0,
            .to = count
        });
    }
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::UI::ListModel<Container, Allocator>::release(void) noexcept
{
    const auto count = _container.size();
    _container.release();
    _dispatcher.dispatch(ListModelEvent::Erase {
        .from = 0,
        .to = count
    });
}

template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::UI::ListModel<Container, Allocator>::move(const Range from, const Range to, const Range out) noexcept
{
    _container.move(from, to, out);
    _dispatcher.dispatch(ListModelEvent::Move {
        .from = from,
        .to = to,
        .out = out
    });
}


template<kF::UI::ListModelContainerRequirements Container, kF::Core::StaticAllocatorRequirements Allocator>
template<typename Compare>
inline void kF::UI::ListModel<Container, Allocator>::sort(Compare &&compare) noexcept
{
    bool invalidated = false;
    _container.sort([compare = std::forward<Compare>(compare), &invalidated](const auto &lhs, const auto &rhs) {
        const auto res = compare(lhs, rhs);
        invalidated |= res;
        return res;
    });
    if (invalidated & !_container.empty()) {
        _dispatcher.dispatch(ListModelEvent::Resize {
            .count = _container.size()
        });
    }
}