#pragma once
#include <string>
#include "Vector.h"

template <typename T>
struct Task {
    std::string op;
    T k;
    Vector<T> a;
    Vector<T> b;

    Task() : op(), k(T()), a(), b() {}
};
