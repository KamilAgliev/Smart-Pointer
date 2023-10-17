#pragma once

#include <exception>

class ControlBlockBase {
public:
    virtual void IncreaseStrong() {
    }

    virtual void DecreaseStrong() {
    }

    virtual void IncreaseWeak() {
    }

    virtual void DecreaseWeak() {
    }

    virtual void OnZeroStrong() {
    }

    virtual void OnZeroWeak() {
    }

    virtual size_t GetStrong() {
    }

    virtual size_t GetWeak() {
        return 0;
    }

    virtual ~ControlBlockBase() {
    }
};

template <class T>
class ControlBlockPtr : public ControlBlockBase {
public:
    ControlBlockPtr(T* ptr) : ptr_(ptr), strong_(1), weak_(0) {
    }

    void IncreaseStrong() override {
        ++strong_;
    }

    void DecreaseStrong() override {
        --strong_;
        OnZeroStrong();
        OnZeroWeak();
    }

    void IncreaseWeak() override {
        ++weak_;
    }

    void DecreaseWeak() override {
        --weak_;
        OnZeroWeak();
    }

    void OnZeroStrong() override {
        if (strong_ == 0) {
            delete ptr_;
            ptr_ = nullptr;
        }
    }

    void OnZeroWeak() override {
        if (strong_ + weak_ == 0) {
            delete this;
        }
    }

    size_t GetStrong() override {
        return strong_;
    }

    size_t GetWeak() override {
        return weak_;
    }

    ~ControlBlockPtr() override {
        if (ptr_) {
            delete ptr_;
        }
    }

    T* ptr_;
    size_t strong_;
    size_t weak_;
};

template <class T>
class ControlBlockArgs : public ControlBlockBase {
public:
    template <class... Args>
    ControlBlockArgs(Args&&... args) : strong_(1), weak_(0) {
        new (&holder) T(std::forward<Args>(args)...);
    }

    void IncreaseStrong() override {
        ++strong_;
    }

    void DecreaseStrong() override {
        --strong_;
        OnZeroStrong();
        OnZeroWeak();
    }

    void IncreaseWeak() override {
        ++weak_;
    }

    void DecreaseWeak() override {
        --weak_;
        OnZeroWeak();
    }

    void OnZeroStrong() override {
        if (strong_ == 0) {
            Get()->~T();
        }
    }

    void OnZeroWeak() override {
        if (strong_ + weak_ == 0) {
            delete this;
        }
    }

    T* Get() {
        return reinterpret_cast<T*>(&holder);
    }

    size_t GetStrong() override {
        return strong_;
    }

    size_t GetWeak() override {
        return weak_;
    }

    ~ControlBlockArgs() override = default;

    size_t strong_;
    size_t weak_;
    alignas(T) char holder[sizeof(T)];
};

class BadWeakPtr : public std::exception {};

class EnableSharedFromThisBase {};

template <class T>
class EnableSharedFromThis;

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;
