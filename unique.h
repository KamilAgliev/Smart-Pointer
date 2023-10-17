#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t

template <class T>
struct Slug {

    void operator()(T* ptr) {
        if (ptr) {
            static_assert(sizeof(T) > 0);
            static_assert(!std::is_void<T>::value);
            delete ptr;
        }
    }

    Slug() {
    }

    template <class D>
    Slug(Slug<D>&&) {
    }

    template <class D>
    Slug& operator=(Slug<D>&&) {
        return *this;
    }
};

template <class T>
struct Slug<T[]> {

    void operator()(T* ptr) {
        if (ptr) {
            static_assert(sizeof(T) > 0);
            static_assert(!std::is_void<T>::value);
            delete[] ptr;
        }
    }

    Slug() {
    }

    template <class D>
    Slug(Slug<D>&&) {
    }

    template <class D>
    Slug& operator=(Slug<D>&&) {
        return *this;
    }
};

// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : pair_(ptr, Deleter()) {
    }

    UniquePtr(T* ptr, Deleter deleter) : pair_(ptr, std::move(deleter)) {
    }

    template <class X, class Y = Slug<X>>
    UniquePtr(UniquePtr<X, Y>&& other) noexcept {
        pair_.GetFirst() = other.Release();
        pair_.GetSecond() = std::move(other.GetDeleter());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        GetDeleter()(pair_.GetFirst());

        pair_.GetFirst() = other.pair_.GetFirst();
        pair_.GetSecond() = std::move(other.pair_.GetSecond());

        other.pair_.GetFirst() = nullptr;

        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        DeletePtr();
        pair_.GetFirst() = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        DeletePtr();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        auto temp = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        return temp;
    }

    void Reset(T* ptr = nullptr) {
        auto temp = pair_.GetFirst();
        pair_.GetFirst() = ptr;
        GetDeleter()(temp);
    }

    void Swap(UniquePtr& other) {
        std::swap(pair_, other.pair_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return pair_.GetFirst();
    }

    Deleter& GetDeleter() {
        return pair_.GetSecond();
    }

    const Deleter& GetDeleter() const {
        return pair_.GetSecond();
    }

    explicit operator bool() const {
        return pair_.GetFirst();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *pair_.GetFirst();
    }

    T* operator->() const {
        return pair_.GetFirst();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Help Functions
    void DeletePtr() {
        if (pair_.GetFirst()) {
            GetDeleter()(pair_.GetFirst());
            pair_.GetFirst() = nullptr;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Variables

    CompressedPair<T*, Deleter> pair_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : pair_(ptr, Deleter()) {
    }

    UniquePtr(T* ptr, Deleter deleter) : pair_(ptr, std::move(deleter)) {
    }

    template <class X, class Y = Slug<X>>
    UniquePtr(UniquePtr<X, Y>&& other) noexcept {
        pair_.GetFirst() = other.Release();
        pair_.GetSecond() = std::move(other.GetDeleter());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        Reset(other.Release());
        pair_.GetSecond() =
            std::forward<Deleter>(other.GetDeleter());  // std::move(other.GetDeleter()) ??
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        DeletePtr();
        pair_.GetSecond() = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        DeletePtr();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        auto temp = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        return temp;
    }

    void Reset(T* ptr = nullptr) {
        auto temp = pair_.GetFirst();
        pair_.GetFirst() = ptr;
        GetDeleter()(temp);
    }

    void Swap(UniquePtr& other) {
        std::swap(pair_, other.pair_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return pair_.GetFirst();
    }

    Deleter& GetDeleter() {
        return pair_.GetSecond();
    }

    const Deleter& GetDeleter() const {
        return pair_.GetSecond();
    }

    explicit operator bool() const {
        return pair_.GetFirst();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *pair_.GetFirst();
    }

    T* operator->() const {
        return pair_.GetFirst();
    }

    std::add_lvalue_reference_t<T> operator[](size_t index) {
        return *(pair_.GetFirst() + index);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Help Functions
    void DeletePtr() {
        if (pair_.GetFirst()) {
            GetDeleter()(pair_.GetFirst());
            pair_.GetFirst() = nullptr;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Variables

    CompressedPair<T*, Deleter> pair_;
};
