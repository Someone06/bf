#ifndef BF_SOURCERATOR_H
#define BF_SOURCERATOR_H

#include <optional>
#include<functional>

template<class T>
class Sourcerator final {
public:
    using value_type = T;
    using difference_type = int;
    using iterator_category = std::input_iterator_tag;

    explicit Sourcerator() = default;

    explicit Sourcerator(std::function<std::optional<T>()> f) : f_(std::move(f)), t_(f_()) {}

    const T& operator*() const {
        return *t_;
    }

    Sourcerator<T>& operator++() {
        t_ = f_();
        return *this;
    }

    Sourcerator<T> operator++(int) {
        auto copy = *this;
        ++*this;
        return copy;
    }

    bool operator==(const Sourcerator& rhs) const {
        return !t_.has_value() && !rhs.t_.has_value();
    }

private:
    std::function<std::optional<T>()> f_;
    std::optional<T> t_;
};

static_assert(std::input_iterator<Sourcerator<int>>);
#endif
