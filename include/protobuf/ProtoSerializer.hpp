#pragma once

#include <memory_management.h>

#include <InternalProtocol.pb.h>

namespace protobuf {

class ProtoSerializer {
public:
	static struct buffer serializeProtobufMessage(const google::protobuf::Message &protobufMessage);

	static InternalProtocol::InternalClient
	createInternalStatus(const struct buffer &statusData, const InternalProtocol::Device &device);

	static InternalProtocol::InternalClient createInternalConnect(const InternalProtocol::Device &device);

	static int checkAndParseConnectResponse(const std::string &internalServerConnectResponse, const InternalProtocol::Device& device);

	static int checkAndParseCommand(std::string *command, const std::string &internalServerCommand,
									const InternalProtocol::Device& device);

};

}