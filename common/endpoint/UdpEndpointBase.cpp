#include "UdpEndpointBase.hpp"

#include <sys/time.h>

#include <chrono>
#include <cstring>
#include <thread>

#include "Logger.hpp"
#include "MessageProcessing.hpp"
#include "ThreadManager.hpp"
#include "defines.hpp"

void UdpEndpointBase::sendEnvelope(const Envelope&   envelope,
                                   const SocketInfo& receiver_info) {
    int64_t n_bytes = sendto(socket_id, &envelope, sizeof(Envelope), 0,
                             const_cast<sockaddr*>(&receiver_info.addr),
                             receiver_info.addrlen);
    LOG("%ld bytes has been sent over UDP", n_bytes);
}

void UdpEndpointBase::sendAck(const SocketInfo& receiver_info,
                              Timestamp         t_stamp) {
    Envelope env{};
    memset(&env, 0, sizeof(Envelope));
    msg_proc::setMessageType(env, MessageType::AckMessage);
    msg_proc::setTimeStamp(env, t_stamp);
    int64_t n_bytes = sendto(socket_id, &env, sizeof(Envelope), 0,
                             const_cast<sockaddr*>(&receiver_info.addr),
                             receiver_info.addrlen);
    LOG("%ld ACK message has been sent over UDP", n_bytes);
}

void UdpEndpointBase::sendEnvelopeAck(const Envelope&   envelope,
                                      const SocketInfo& receiver_info) {
    // Timestamp envelope before to send
    timeval curTime{};
    gettimeofday(&curTime, NULL);
    Timestamp tStamp = (static_cast<uint32_t>(curTime.tv_sec) << 20) |
                       (static_cast<uint32_t>(curTime.tv_usec));

    Envelope envStamped = envelope;
    msg_proc::setTimeStamp(envStamped, tStamp);

    ThreadManager::getInstance().schedule_task(
        [this, envStamped, receiver_info, tStamp]() mutable {
            // Send timestamped envelope
            bool acknowledge_received = false;

            for (uint8_t attempt = 0;
                 (attempt < 10) and (!acknowledge_received); ++attempt) {
                sendEnvelope(envStamped, receiver_info);
                LOG("Envelope with timestamp %lu has been sent", tStamp);
                for (uint8_t count = 0; count < 10; count++) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    {
                        std::unique_lock<std::mutex> lock(ackMtx);
                        auto response = this->ackEnvelope;
                        // Check if ACK is correct by timestamps comparation
                        if ((response) and
                            (msg_proc::getTimeStamp(*response) == tStamp)) {
                            acknowledge_received = true;
                            this->ackEnvelope    = std::nullopt;
                            LOG("Transmission succeed.");
                            break;
                        }
                    }
                }
            }
            if (!acknowledge_received) {
                printf(
                    "Message is not received; check network connection and try "
                    "to send "
                    "message again.\n");
                LOG("MESSAGE TRANSMISSION FAILED: NO ACK RECEIVED");
            }
        });
}

Envelope UdpEndpointBase::receiveEnvelope(const SocketInfo* sender_info) {
    Envelope env{};
    memset(&env, 0, sizeof(Envelope));
    int64_t n_bytes = 0;
    if (sender_info != nullptr) {
        n_bytes = recvfrom(socket_id, &env, sizeof(Envelope), 0,
                           const_cast<sockaddr*>(&(sender_info->addr)),
                           const_cast<socklen_t*>(&(sender_info->addrlen)));
    } else {
        n_bytes = recv(socket_id, &env, sizeof(Envelope), 0);
    }

    if (msg_proc::getMessageType(env) != MessageType::AckMessage) {
        LOG("%ld bytes of non-ACK message received", n_bytes);
        if (sender_info != nullptr) {
            // if sender is known - send ACK message back
            sendAck(*sender_info, msg_proc::getTimeStamp(env));
        }
    } else {
        LOG("ACK message with timestamp %lu received",
            msg_proc::getTimeStamp(env));
        std::unique_lock<std::mutex> lock(ackMtx);
        updateAckEnvelope(env);
    }

    return env;
}

std::optional<Envelope> UdpEndpointBase::tryReceiveEnvelope(
    const SocketInfo* sender_info) {
    const auto [ready_to_read, err_or_closed] = pollSocket(socket_id);
    if (ready_to_read) {
        return receiveEnvelope(sender_info);
    }
    return std::nullopt;
}
