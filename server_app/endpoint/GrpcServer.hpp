#pragma once
#include <grpc/grpc.h>
#include <grpcpp/server_context.h>

#include "grpc_services.grpc.pb.h"
#include "proto_defines.pb.h"
//#include <grpcpp/impl/channel_interface.h>

#include <CircularBuffer.hpp>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "defines.hpp"

using google::protobuf::Empty;
using grpc::ServerContext;
using grpc::Status;

using PostBoxCell = CircularBuffer<Envelope, 32>;

class GrpcEndpointServer final : public grpc_service::GrpcEndpoint::Service {
   public:
    GrpcEndpointServer() : post_box() {}
    Status acceptConnection(ServerContext*                       context,
                            const protobuf_itf::UserCredentials* user_creds,
                            grpc_service::UserID* user_id) override;
    Status receiveMessage(ServerContext*                context,
                          const protobuf_itf::Envelope* p_env,
                          Empty*                        nothing_ptr) override;

    Status streamIncomingMessages(
        ServerContext* context, const Empty* nothing_ptr,
        grpc::ServerWriter<protobuf_itf::Envelope>* writer) override;

    void addUserToConnected(const UserID& new_user);
    void disconnectUser(const UserID& user);
    bool isOnline(const UserID& user);

   private:
    std::unordered_map<UserID, PostBoxCell> post_box;
    std::shared_mutex                       post_box_mtx;
};
