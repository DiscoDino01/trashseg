#pragma once

template <typename T>
class AssignOnce {
    T value;       // Holds the actual value
    bool is_set;   // Tracks whether the value has been set

public:
    AssignOnce() : is_set(false) {}

    void set(const T& val) {
        if (is_set) {
            throw std::runtime_error("Value is already set and cannot be changed.");
        }
        value = val;
        is_set = true;
    }

    const T& get() const {
        if (!is_set) {
            throw std::runtime_error("Value is not set yet.");
        }
        return value;
    }

    bool has_set() {
        return is_set;
    }
};