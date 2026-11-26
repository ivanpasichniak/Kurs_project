#pragma once
#include <stdexcept>
#include <string>
#include "Vector.h"

template <typename T>
class Transform {
public:
    virtual ~Transform() {}

    Vector<T> execute(const Vector<T>& v) const {
        if (v.size() <= 0) {
            throw std::invalid_argument("invalid size");
        }
        return apply(v);
    }

    virtual Vector<T> apply(const Vector<T>& v) const = 0;
};

template <typename T>
class VectorReflect : public Transform<T> {
public:
    Vector<T> apply(const Vector<T>& v) const override {
        const int n = v.size();
        Vector<T> c(n);
        for (int j = 0; j < n; ++j) {
            c[j] = v[n - 1 - j];
        }
        return c;
    }
};

template <typename T>
class VectorScale : public Transform<T> {
private:
    T k_;

public:
    explicit VectorScale(T k) : k_(k) {}

    Vector<T> apply(const Vector<T>& v) const override {
        const int n = v.size();
        Vector<T> c(n);
        for (int j = 0; j < n; ++j) {
            c[j] = k_ * v[j];
        }
        return c;
    }
};

template <typename T>
class VectorAdd : public Transform<T> {
private:
    Vector<T> second_;

public:
    explicit VectorAdd(const Vector<T>& second) : second_(second) {}

    Vector<T> apply(const Vector<T>& v) const override {
        if (v.size() != second_.size()) {
            throw std::invalid_argument(
                "vector sizes differ: " + std::to_string(v.size()) +
                " and " + std::to_string(second_.size()));
        }
        const int n = v.size();
        Vector<T> c(n);
        for (int j = 0; j < n; ++j) {
            c[j] = v[j] + second_[j];
        }
        return c;
    }
};
