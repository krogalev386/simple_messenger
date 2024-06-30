#pragma once

#include "defines.hpp"

#include <cstdint>
#include <cstddef>
#include <tuple>


//constexpr size_t max_payload_size = 1024 - sizeof(Header);
//
//struct DataPacket
//{
//    Header header;
//    char payload[max_payload_size];
//};

std::tuple<size_t> parse_header(void* message)
{
    Header* hdr = reinterpret_cast<Header*>(message);
    return std::make_tuple(hdr->payload_size);
};