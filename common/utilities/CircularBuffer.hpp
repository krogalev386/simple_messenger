#pragma once

#include <array>
#include <cstdint>
#include <mutex>
#include <optional>

#include "Logger.hpp"

template <typename T, size_t length>
class CircularBuffer {
   private:
    std::array<T, length> circ_buffer;
    std::mutex            rw_mtx, ronly_mtx;

   public:
    size_t begin_pos;
    size_t end_pos;
    size_t size;
    CircularBuffer() : begin_pos(0), end_pos(0), size(0){};
    ~CircularBuffer() = default;

    CircularBuffer(const CircularBuffer&)           = delete;
    // CircularBuffer(CircularBuffer&&)                = delete;
    CircularBuffer& operator=(const CircularBuffer) = delete;

    CircularBuffer(CircularBuffer&& other) {
        std::unique_lock<std::mutex> lock(rw_mtx);
        if (this != &other) {
            circ_buffer = std::move(other.circ_buffer);
        }
    };

    bool is_empty() {
        std::unique_lock<std::mutex> lock(ronly_mtx);
        return size == 0;
    };

    bool is_full() {
        std::unique_lock<std::mutex> lock(ronly_mtx);
        return size == length;
    };

    T& operator[](int offset) {
        std::unique_lock<std::mutex> lock(rw_mtx);
        offset         = offset % size;
        size_t ret_pos = begin_pos + offset;
        if (ret_pos >= size) {
            ret_pos = ret_pos % size;
        }
        return circ_buffer[ret_pos];
    };

    bool push(const T& item) {
        std::unique_lock<std::mutex> lock(rw_mtx);
        if (not is_full()) {
            circ_buffer[end_pos] = item;
            end_pos++;
            if (end_pos == length) {
                end_pos = 0;
            }
            size++;
            LOG("New size: %u", size);
            return true;
        } else {
            LOG("Error! Circular buffer overflow!");
            return false;
        }
    }

    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(rw_mtx);
        if (is_empty()) {
            // LOG("Empty buffer!");
            return std::nullopt;
        }
        T item = circ_buffer[begin_pos];
        size--;
        begin_pos++;
        if (begin_pos == length) {
            begin_pos = 0;
        }
        return std::move(item);
    }
};
