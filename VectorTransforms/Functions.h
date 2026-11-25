#pragma once
#include <climits>
#include <cmath>
#include <iomanip>
#include <istream>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "Task.h"
#include "Transform.h"

inline void stripCR(std::string& s) {
    if (!s.empty() && s.back() == '\r') {
        s.pop_back();
    }
}

inline std::string trim(const std::string& s) {
    const std::string ws = " \t\r\n";
    const std::size_t first = s.find_first_not_of(ws);
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = s.find_last_not_of(ws);
    return s.substr(first, last - first + 1);
}

inline std::vector<std::string> splitTokens(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string tok;
    while (iss >> tok) {
        tokens.push_back(tok);
    }
    return tokens;
}

template <typename T>
bool parseNumber(const std::string& text, T& value) {
    std::istringstream iss(text);
    iss.imbue(std::locale::classic());
    T parsed = T();
    iss >> parsed;
    if (iss.fail()) {
        return false;
    }
    char extra;
    if (iss >> extra) {
        return false;
    }
    value = parsed;
    return true;
}

inline bool parseSize(const std::string& text, int& sizeOut) {
    long long parsed = 0;
    if (!parseNumber<long long>(text, parsed)) {
        return false;
    }
    if (parsed < 1 || parsed > INT_MAX) {
        return false;
    }
    sizeOut = static_cast<int>(parsed);
    return true;
}

template <typename T>
bool parseVectorLine(const std::string& line, Vector<T>& out, std::string& error) {
    const std::vector<std::string> tokens = splitTokens(line);
    if (tokens.empty()) {
        error = "invalid size";
        return false;
    }
    int n = 0;
    if (!parseSize(tokens[0], n)) {
        error = "invalid size";
        return false;
    }
    if (static_cast<int>(tokens.size()) - 1 != n) {
        error = "wrong number of elements";
        return false;
    }
    Vector<T> result(n);
    for (int i = 0; i < n; ++i) {
        T val = T();
        if (!parseNumber<T>(tokens[i + 1], val)) {
            error = "invalid number";
            return false;
        }
        result[i] = val;
    }
    out = result;
    return true;
}

template <typename T>
void parseBlock(const std::vector<std::string>& block, Task<T>& task, std::string& error) {
    const std::vector<std::string> header = splitTokens(block[0]);
    if (header.empty()) {
        error = "unknown operation";
        return;
    }

    const std::string& op = header[0];
    if (op != "REFLECT" && op != "SCALE" && op != "ADD") {
        error = "unknown operation";
        return;
    }
    task.op = op;

    if (op == "SCALE") {
        if (header.size() < 2) {
            error = "invalid constant";
            return;
        }
        if (header.size() > 2) {
            error = "unexpected parameter";
            return;
        }
        if (!parseNumber<T>(header[1], task.k)) {
            error = "invalid constant";
            return;
        }
    } else {
        if (header.size() > 1) {
            error = "unexpected parameter";
            return;
        }
    }

    if (block.size() < 2) {
        error = "invalid size";
        return;
    }

    if (!parseVectorLine<T>(block[1], task.a, error)) {
        return;
    }

    if (op == "ADD") {
        if (block.size() < 3) {
            error = "missing second vector";
            return;
        }
        if (block.size() > 3) {
            error = "unexpected parameter";
            return;
        }
        if (!parseVectorLine<T>(block[2], task.b, error)) {
            return;
        }
    } else {
        if (block.size() > 2) {
            error = "unexpected parameter";
            return;
        }
    }
}

template <typename T>
bool readBlock(std::istream& in, Task<T>& task, std::string& error) {
    task = Task<T>();
    error.clear();
    std::string line;
    bool found = false;
    while (std::getline(in, line)) {
        stripCR(line);
        if (!trim(line).empty()) {
            found = true;
            break;
        }
    }
    if (!found || trim(line) == "END") {
        return false;
    }
    std::vector<std::string> block(1, line);
    while (std::getline(in, line)) {
        stripCR(line);
        if (trim(line).empty()) {
            break;
        }
        block.push_back(line);
    }
    parseBlock(block, task, error);
    return true;
}

template <typename T>
std::unique_ptr<Transform<T>> createTransform(const Task<T>& task) {
    if (task.op == "REFLECT") {
        return std::make_unique<VectorReflect<T>>();
    }
    if (task.op == "SCALE") {
        return std::make_unique<VectorScale<T>>(task.k);
    }
    if (task.op == "ADD") {
        return std::make_unique<VectorAdd<T>>(task.b);
    }
    throw std::invalid_argument("unknown operation");
}

template <typename T>
std::string formatValue(T value) {
    double v = static_cast<double>(value);
    if (std::fabs(v) < 0.00005) {
        v = 0.0;
    }
    std::ostringstream oss;
    oss.imbue(std::locale::classic());
    oss << std::fixed << std::setprecision(4) << v;
    return oss.str();
}

template <typename T>
std::string formatConstant(T value) {
    double v = static_cast<double>(value);
    if (v == 0.0) {
        v = 0.0;
    }
    std::ostringstream oss;
    oss.imbue(std::locale::classic());
    oss << v;
    return oss.str();
}

template <typename T>
void writeResult(std::ostream& out, int number, const Task<T>& task, const Vector<T>& c) {
    out << number << " " << task.op;
    if (task.op == "SCALE") {
        out << " " << formatConstant(task.k);
    }
    out << " =";
    for (int i = 0; i < c.size(); ++i) {
        out << " " << formatValue(c[i]);
    }
    out << "\n";
}

inline void writeError(std::ostream& out, int number, const std::string& message) {
    out << number << " ERROR: " << message << "\n";
}

template <typename T>
int processAll(std::istream& in, std::ostream& file, std::ostream& screen, int startNumber = 0) {
    Task<T> task;
    std::string error;
    int number = startNumber;
    while (readBlock(in, task, error)) {
        ++number;
        Vector<T> c;
        bool ok = false;
        std::string message = error;
        if (message.empty()) {
            try {
                auto transform = createTransform(task);
                c = transform->execute(task.a);
                ok = true;
            } catch (const std::exception& e) {
                message = e.what();
            }
        }
        if (ok) {
            writeResult(file, number, task, c);
            writeResult(screen, number, task, c);
        } else {
            writeError(file, number, message);
            writeError(screen, number, message);
        }
    }
    return number - startNumber;
}
