#include "GrpcServer.hpp"

#include <grpc/grpc.h>
#include <grpcpp/server_context.h>

#include "grpc_services.grpc.pb.h"
#include "proto_defines.pb.h"
//#include <grpcpp/impl/channel_interface.h>

#include <algorithm>
#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

#include "AuthentificationService.hpp"
#include "Logger.hpp"
#include "MessageProcessing.hpp"
#include "PsqlManager.hpp"
#include "defines.hpp"

using google::protobuf::Empty;
using grpc::ServerContext;
using grpc::Status;

Status GrpcEndpointServer::acceptConnection(
    ServerContext* /*context*/, const protobuf_itf::UserCredentials* user_creds,
    grpc_service::UserID* user_id) {
    UserCredentials received_creds{};
    std::string     s = user_creds->DebugString();
    LOG("Received message: %s", s.c_str());
    memcpy(&received_creds.email, (user_creds->email()).c_str(),
           (user_creds->email()).size());
    memcpy(&received_creds.password, (user_creds->password()).c_str(),
           (user_creds->password()).size());
    LOG("User email: %s", received_creds.email);
    auto user_id_opt = AuthentificationService::getInstance().checkIfRegistered(
        received_creds);
    if (user_id_opt) {
        addUserToConnected(user_id_opt.value());
        user_id->set_id(user_id_opt.value());
        return Status::OK;
    }
    return Status::CANCELLED;
};

Status GrpcEndpointServer::receiveMessage(ServerContext* /*context*/,
                                          const protobuf_itf::Envelope* p_env,
                                          Empty* /*nothing_ptr*/) {
    // std::string s = p_env->DebugString();
    // LOG("Received message: %s", s.c_str());
    Envelope received_env = msg_proc::getEnvFromProtoBuf(*p_env);
    UserID   recipient_id = msg_proc::getRecepientID(received_env);
    UserID   sender_id    = msg_proc::getSenderID(received_env);

    if (std::string(received_env.payload) == std::string("exit")) {
        disconnectUser(sender_id);
        LOG("User %d successfully logged out", sender_id);
        return Status::OK;
    }
    if (not isOnline(recipient_id)) {
        LOG("Recipient %d is offline", recipient_id);
        return Status::CANCELLED;
    }
    if (post_box[recipient_id].push(received_env)) {
        return Status::OK;
    }
    LOG("Failed to save message for user_id %d: no space left in the post "
        "box!",
        recipient_id);
    return Status::CANCELLED;
};

Status GrpcEndpointServer::streamIncomingMessages(
    ServerContext* context, const Empty* /*nothing_ptr*/,
    grpc::ServerWriter<protobuf_itf::Envelope>* writer) {
    auto   metadata = context->client_metadata();
    UserID user_id  = 0;
    for (auto iter = metadata.begin(); iter != metadata.end(); ++iter) {
        auto key   = std::string((iter->first).data(), (iter->first).size());
        auto value = std::string((iter->second).data(), (iter->second).size());
        LOG("%s: %s", key.c_str(), value.c_str());
        if (key == std::string("user_id")) {
            user_id = std::stoul(value);
            break;
        }
    }

    protobuf_itf::Envelope forwardedEnv;

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        {
            if (not isOnline(user_id)) {
                LOG("User %d is disconnected", user_id);
                return Status::OK;
            }
            auto env_to_forward = post_box[user_id].pop();
            if (env_to_forward) {
                msg_proc::putEnvToProtoBuf(env_to_forward.value(),
                                           forwardedEnv);
                writer->Write(forwardedEnv);
            }
        }
    }
    return Status::OK;
};

void GrpcEndpointServer::addUserToConnected(const UserID& new_user) {
    std::unique_lock<std::shared_mutex> lock(post_box_mtx);
    post_box.emplace(new_user, PostBoxCell());
};

void GrpcEndpointServer::disconnectUser(const UserID& user) {
    std::unique_lock<std::shared_mutex> lock(post_box_mtx);
    post_box.erase(user);
};

bool GrpcEndpointServer::isOnline(const UserID& user) {
    std::shared_lock<std::shared_mutex> lock(post_box_mtx);
    return (post_box.find(user) != post_box.end());
};
