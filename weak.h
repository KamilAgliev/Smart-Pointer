#pragma once

#include "sw_fwd.h"  // Forward declaration
#include <utility>

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() {
    }

    WeakPtr(const WeakPtr& other) {
        block_ = other.block_;
        if (block_) {
            block_->IncreaseWeak();
        }
        ptr_ = other.ptr_;
    }

    WeakPtr(WeakPtr&& other) {
        block_ = other.block_;
        ptr_ = other.ptr_;
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T>& other) {
        block_ = other.block_;
        if (block_) {
            block_->IncreaseWeak();
        }
        ptr_ = other.ptr_;
    }

    WeakPtr(T* ptr, ControlBlockBase* block) : ptr_(ptr), block_(block) {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        if (this == &other) {
            return *this;
        }
        if (block_) {
            block_->DecreaseWeak();
        }
        block_ = other.block_;
        ptr_ = other.ptr_;
        if (block_) {
            block_->IncreaseWeak();
        }
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        if (this == &other) {
            return *this;
        }
        if (block_) {
            block_->DecreaseWeak();
        }
        block_ = other.block_;
        ptr_ = other.ptr_;
        other.block_ = nullptr;
        other.ptr_ = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        if (block_) {
            block_->DecreaseWeak();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            block_->DecreaseWeak();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    }

    void Swap(WeakPtr& other) {
        std::swap(block_, other.block_);
        std::swap(ptr_, other.ptr_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const {
        if (block_) {
            return block_->GetStrong();
        }
        return 0;
    }

    bool Expired() const {
        if (!block_) {
            return true;
        }
        return block_->GetStrong() == 0;
    }

    SharedPtr<T> Lock() const {
        return Expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
    }

    ControlBlockBase* block_ = nullptr;
    T* ptr_ = nullptr;
};
