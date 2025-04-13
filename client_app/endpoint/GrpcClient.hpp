#pragma once
#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
#include <grpcpp/impl/channel_interface.h>

#include <memory>

#include "defines.hpp"
#include "grpc_services.grpc.pb.h"
#include "proto_defines.pb.h"

class GrpcEndpointClient {
   public:
    GrpcEndpointClient(std::shared_ptr<grpc::ChannelInterface> channel,
                       const std::string&                      user_name)
        : stub_(grpc_service::GrpcEndpoint::NewStub(channel)),
          user_name(user_name),
          user_id(0){};

    bool   connectToServer(const UserCredentials& user_creds);
    bool   sendEnvelope(const protobuf_itf::Envelope& env);
    void   listenOthers();
    UserID getUserID() const { return user_id; };

   private:
    std::unique_ptr<grpc_service::GrpcEndpoint::Stub> stub_;
    std::string                                       user_name;
    UserID                                            user_id;
};
