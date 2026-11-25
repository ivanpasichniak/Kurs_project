#pragma once
#include <stdexcept>

template <typename T>
class Vector {
private:
    T* data_;
    int count_;

    void checkIndex(int i) const {
        if (i < 0 || i >= count_) {
            throw std::out_of_range("index out of range");
        }
    }

public:
    explicit Vector(int count = 0) : data_(nullptr), count_(0) {
        if (count < 0) {
            throw std::invalid_argument("invalid size");
        }
        if (count > 0) {
            data_ = new T[count]();
            count_ = count;
        }
    }

    Vector(const Vector& other) : data_(nullptr), count_(0) {
        if (other.count_ > 0) {
            data_ = new T[other.count_];
            count_ = other.count_;
            for (int i = 0; i < count_; ++i) {
                data_[i] = other.data_[i];
            }
        }
    }

    ~Vector() {
        delete[] data_;
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }
        T* newData = nullptr;
        if (other.count_ > 0) {
            newData = new T[other.count_];
            for (int i = 0; i < other.count_; ++i) {
                newData[i] = other.data_[i];
            }
        }
        delete[] data_;
        data_ = newData;
        count_ = other.count_;
        return *this;
    }

    int size() const {
        return count_;
    }

    T& operator[](int i) {
        checkIndex(i);
        return data_[i];
    }

    const T& operator[](int i) const {
        checkIndex(i);
        return data_[i];
    }
};
