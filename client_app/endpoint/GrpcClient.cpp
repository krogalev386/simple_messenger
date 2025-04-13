#include "GrpcClient.hpp"

#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
#include <grpcpp/impl/channel_interface.h>

#include <iostream>
#include <memory>
#include <string>

#include "Logger.hpp"
#include "MessageProcessing.hpp"
#include "defines.hpp"
#include "grpc_services.grpc.pb.h"
#include "proto_defines.pb.h"

using google::protobuf::Empty;
using grpc::ClientContext;
using grpc::Status;

bool GrpcEndpointClient::connectToServer(const UserCredentials& user_creds) {
    ClientContext context;

    protobuf_itf::UserCredentials proto_user_creds;
    proto_user_creds.set_email(user_creds.email);
    proto_user_creds.set_password(user_creds.password);

    grpc_service::UserID proto_user_id;
    Status               status =
        stub_->acceptConnection(&context, proto_user_creds, &proto_user_id);
    if (status.ok()) {
        this->user_id = proto_user_id.id();
        return true;
    }
    return false;
};

bool GrpcEndpointClient::sendEnvelope(const protobuf_itf::Envelope& env) {
    ClientContext context;
    Empty         nothing;
    Status        status = stub_->receiveMessage(&context, env, &nothing);
    return status.ok();
};

void GrpcEndpointClient::listenOthers() {
    ClientContext context;
    Empty         nothing;

    context.AddMetadata("username", this->user_name);
    context.AddMetadata("user_id", std::to_string(this->user_id));
    std::unique_ptr<grpc::ClientReader<protobuf_itf::Envelope>> reader(
        stub_->streamIncomingMessages(&context, nothing));
    protobuf_itf::Envelope received_message;
    while (reader->Read(&received_message)) {
        Envelope env = msg_proc::getEnvFromProtoBuf(received_message);
        char     char_data[sizeof(env.payload)];
        memcpy(char_data, &env.payload, sizeof(char_data));
        std::string data = std::string(char_data);
        std::cout << "Incoming message successfully received: " << char_data
                  << std::endl;
    }
    Status status = reader->Finish();
};
