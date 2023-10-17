#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <utility>
#include <memory>

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() {
    }

    SharedPtr(std::nullptr_t) {
    }

    explicit SharedPtr(T* ptr) {
        block_ = new ControlBlockPtr<T>(ptr);
        ptr_ = ptr;
        if constexpr (std::is_base_of_v<EnableSharedFromThisBase, T>) {
            ptr_->ptr_ = ptr;
            ptr_->block_ = block_;
        }
    }

    template <class U>
    explicit SharedPtr(U* ptr) {
        block_ = new ControlBlockPtr<U>(ptr);
        ptr_ = static_cast<U*>(ptr);
        if constexpr (std::is_base_of_v<EnableSharedFromThisBase, U>) {
            ptr_->ptr_ = static_cast<U*>(ptr);
            ptr_->block_ = block_;
        }
    }

    SharedPtr(T* ptr, ControlBlockBase* block) : ptr_(ptr), block_(block) {
        if constexpr (std::is_base_of_v<EnableSharedFromThisBase, T>) {
            ptr_->ptr_ = ptr;
            ptr_->block_ = block_;
        }
    }

    template <class X>
    SharedPtr(const SharedPtr<X>& other) {
        block_ = other.block_;
        if (block_) {
            block_->IncreaseStrong();
        }
        ptr_ = other.ptr_;
    }

    template <class X>
    SharedPtr(SharedPtr<X>&& other) {
        block_ = other.block_;
        ptr_ = other.ptr_;
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    SharedPtr(const SharedPtr<T>& other) {
        block_ = other.block_;
        if (block_) {
            block_->IncreaseStrong();
        }
        ptr_ = other.ptr_;
    }

    SharedPtr(SharedPtr<T>&& other) {
        block_ = other.block_;
        ptr_ = other.ptr_;
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) {
        block_ = other.block_;
        if (block_) {
            block_->IncreaseStrong();
        }
        ptr_ = ptr;
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        block_ = other.block_;
        if (block_) {
            block_->IncreaseStrong();
        }
        ptr_ = other.ptr_;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    template <class X>
    SharedPtr& operator=(const SharedPtr<X>& other) {
        if (block_) {
            block_->DecreaseStrong();
        }
        block_ = other.block_;
        if (block_) {
            block_->IncreaseStrong();
        }
        ptr_ = other.ptr_;
        return *this;
    }

    template <class X>
    SharedPtr& operator=(SharedPtr<X>&& other) {
        if (block_) {
            block_->DecreaseStrong();
        }
        block_ = other.block_;
        ptr_ = other.ptr_;
        other.block_ = nullptr;
        other.ptr_ = nullptr;
        return *this;
    }

    SharedPtr& operator=(const SharedPtr<T>& other) {
        if (this == &other) {
            return *this;
        }
        if (block_) {
            block_->DecreaseStrong();
        }
        block_ = other.block_;
        if (block_) {
            block_->IncreaseStrong();
        }
        ptr_ = other.ptr_;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        if (block_) {
            block_->DecreaseStrong();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            block_->DecreaseStrong();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    }

    void Reset(T* ptr) {
        Reset();
        block_ = new ControlBlockPtr<T>(ptr);
        ptr_ = ptr;
    }

    template <class U>
    void Reset(U* ptr) {
        Reset();
        block_ = new ControlBlockPtr<U>(ptr);
        ptr_ = ptr;
    }

    void Swap(SharedPtr& other) {
        std::swap(block_, other.block_);
        std::swap(ptr_, other.ptr_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    size_t UseCount() const {
        if (!block_) {
            return 0;
        }
        return block_->GetStrong();
    }

    explicit operator bool() const {
        return block_;
    }

    ControlBlockBase* block_ = nullptr;
    T* ptr_ = nullptr;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return left.Get() == right.Get();
}

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    ControlBlockArgs<T>* block = new ControlBlockArgs<T>(std::forward<Args>(args)...);
    return SharedPtr<T>(block->Get(), block);
}

// Look for usage examples in tests
template <typename T>
class EnableSharedFromThis : EnableSharedFromThisBase {
public:
    SharedPtr<T> SharedFromThis() {
        SharedPtr<T> ptr(ptr_, block_);
        if (block_) {
            block_->IncreaseStrong();
        }
        return ptr;
    }

    SharedPtr<const T> SharedFromThis() const {
        SharedPtr<const T> ptr(ptr_, block_);
        if (block_) {
            block_->IncreaseStrong();
        }
        return ptr;
    }

    WeakPtr<T> WeakFromThis() noexcept {
        WeakPtr<T> ptr(ptr_, block_);
        if (block_) {
            block_->IncreaseWeak();
        }
        return ptr;
    }

    WeakPtr<const T> WeakFromThis() const noexcept {
        WeakPtr<const T> ptr(ptr_, block_);
        if (block_) {
            block_->IncreaseWeak();
        }
        return ptr;
    }

    ControlBlockBase* block_ = nullptr;
    T* ptr_ = nullptr;
};
