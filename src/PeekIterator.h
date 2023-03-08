#ifndef BF_PEEKITERATOR_H
#define BF_PEEKITERATOR_H

#include <iterator>

template<std::input_iterator T, typename S>
requires std::is_move_constructible_v<T>
             && std::is_copy_assignable_v<std::iter_value_t<T>>
             && std::is_copy_assignable_v<std::iter_value_t<T>>
             && std::equality_comparable_with<std::iter_value_t<T>, S>
             && std::move_constructible<S>
class PeekIterator final {
public:
    using value_type = std::iter_value_t<T>;
    using difference_type = std::iter_difference_t<T>;
    using iterator_category = std::input_iterator_tag;

    PeekIterator(T iterator, S sentinal) : iter{std::move(iterator)}, s{std::move(sentinal)}, val{*iter} {}

    const T& operator*() const {
        return val;
    }

    PeekIterator<T, S>& operator++() {
        if(has_next()) {
            val = *iter;
            ++iter;
        }
        return *this;
    }

    PeekIterator<T, S> operator++(int) {
        auto copy = *this;
        ++*this;
        return copy;
    }

    bool operator==(const PeekIterator<T, S>& rhs) const {
        return !has_next() && !has_next();
    }

    value_type& peek() const {
        if(has_next()) { return *iter; }
        throw std::range_error("Cannot peek beyond end of iterator");
    }

    bool has_next() const {
        return val != s;
    }

    bool operator()() const {
        return has_next();
    }

private:
    T iter;
    S s;
    value_type val;
};
#endif
