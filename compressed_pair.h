#pragma once

#include <type_traits>
#include <memory>

template <typename F, typename S,
          bool type = (std::is_empty_v<F> && !std::is_base_of_v<F, S> && !std::is_final_v<F>),
          bool type2 = (std::is_empty_v<S> && !std::is_base_of_v<S, F> && !std::is_final_v<S>)>
class CompressedPair;

template <typename F, typename S>
class CompressedPair<F, S, true, true> : F, S {
public:
    CompressedPair() {
    }

    CompressedPair(const F&, const S&) {
    }

    CompressedPair(const F&, S&&) {
    }

    CompressedPair(F&&, const S&) {
    }

    CompressedPair(F&&, S&&) {
    }

    F& GetFirst() {
        return *this;
    }

    const F& GetFirst() const {
        return *this;
    }

    S& GetSecond() {
        return *this;
    };

    const S& GetSecond() const {
        return *this;
    };

private:
};

template <typename F, typename S>
class CompressedPair<F, S, true, false> : F {
public:
    CompressedPair() : second_(S()) {
    }

    CompressedPair(const F&, const S& second) : second_(second) {
    }

    CompressedPair(const F&, S&& second) : second_(std::move(second)) {
    }

    CompressedPair(F&&, const S& second) : second_(second) {
    }

    CompressedPair(F&&, S&& second) : second_(std::move(second)) {
    }

    F& GetFirst() {
        return *this;
    }

    const F& GetFirst() const {
        return *this;
    }

    S& GetSecond() {
        return second_;
    };

    const S& GetSecond() const {
        return second_;
    };

private:
    S second_;
};

template <typename F, typename S>
class CompressedPair<F, S, false, true> : S {
public:
    CompressedPair() : first_(F()) {
    }

    CompressedPair(const F& first, const S&) : first_(first) {
    }

    CompressedPair(const F& first, S&&) : first_(std::move(first)) {
    }

    CompressedPair(F&& first, const S&) : first_(first) {
    }

    CompressedPair(F&& first, S&&) : first_(std::move(first)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return *this;
    };

    const S& GetSecond() const {
        return *this;
    };

private:
    F first_;
};

template <typename F, typename S>
class CompressedPair<F, S, false, false> {
public:
    CompressedPair() : first_(F()), second_(S()) {
    }

    CompressedPair(const F& first, const S& second) : first_(first), second_(second) {
    }

    CompressedPair(const F& first, S&& second) : first_(first), second_(std::move(second)) {
    }

    CompressedPair(F&& first, const S& second) : first_(std::move(first)), second_(second) {
    }

    CompressedPair(F&& first, S&& second) : first_(std::move(first)), second_(std::move(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return second_;
    };

    const S& GetSecond() const {
        return second_;
    };

private:
    F first_;
    S second_;
};