#pragma once

#include "Logger.hpp"

#include <array>
#include <cstdint>
#include <optional>

template<typename T, size_t length>
class CircularBuffer
{
private:
    std::array<T, length> circ_buffer;
public:
    size_t begin_pos;
    size_t end_pos;
    size_t size;
    CircularBuffer() : begin_pos(0), end_pos(0), size(0) {};
    ~CircularBuffer() = default;

    CircularBuffer(const CircularBuffer&)           = delete;
    CircularBuffer(CircularBuffer&&)                = delete;
    CircularBuffer& operator=(const CircularBuffer) = delete;

    bool is_empty()
    {
        return size == 0;
    };

    bool is_full()
    {
        return size == length;
    };

    T& operator[](int offset)
    {
        offset = offset % size;
        size_t ret_pos = begin_pos + offset;
        if (ret_pos >= size)
        {
            ret_pos = ret_pos % size;
        }
        return circ_buffer[ret_pos];
    };

    bool push(const T& item)
    {
        if (not is_full())
        {
            circ_buffer[end_pos] = item;
            end_pos++;
            if (end_pos == length)
            {
                end_pos = 0;
            }
            size++;
            LOG("New size: %u", size);
            return true;
        }
        else
        {
            LOG("Error! Circular buffer overflow!");
            return false;
        }
    }

    std::optional<T> pop()
    {
        if (is_empty())
        {
            LOG("Empty buffer!");
            return std::nullopt;
        }
        T item = circ_buffer[begin_pos];
        size--;
        begin_pos++;
        if (begin_pos == length)
        {
            begin_pos = 0;
        }
        return std::move(item);
    }
};
