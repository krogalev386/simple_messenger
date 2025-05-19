#include "MessageProcessing.hpp"
#include "gtest/gtest.h"
#include "defines.hpp"

namespace {

TEST(MessageProcessing, processPayload) {
    uint64_t expected_message = 412397841;
    Envelope env{};
    msg_proc::set_payload<uint64_t>(env, expected_message);
    uint64_t result_message = msg_proc::get_payload<uint64_t>(env);
    EXPECT_EQ(expected_message, result_message);
};

}