#include <protobuf/ProtoSerializer.hpp>
#include <general_error_codes.h>
#include <internal_client.h>

#include <google/protobuf/util/message_differencer.h>

namespace protobuf {
struct buffer ProtoSerializer::serializeProtobufMessage(const google::protobuf::Message &protobufMessage) {
	struct buffer message {};
	message.size_in_bytes = protobufMessage.ByteSizeLong();
	message.data = malloc(message.size_in_bytes);
	protobufMessage.SerializeToArray(message.data, message.size_in_bytes);

	return message;
}

InternalProtocol::InternalClient
ProtoSerializer::createInternalStatus(const struct buffer &statusData, const InternalProtocol::Device &device) {
	InternalProtocol::InternalClient internalClientMessage;

	InternalProtocol::DeviceStatus *deviceStatus = internalClientMessage.mutable_devicestatus();
	InternalProtocol::Device *devicePtr = deviceStatus->mutable_device();
	devicePtr->CopyFrom(device);

	deviceStatus->set_statusdata(statusData.data, statusData.size_in_bytes);

	return internalClientMessage;
}


int ProtoSerializer::checkAndParseCommand(std::string *command, const std::string &internalServerCommand,
										  const InternalProtocol::Device& device) {
	if(internalServerCommand.empty()) {
		return NOT_OK;
	}
	InternalProtocol::InternalServer internalServerMessage;

	if (!internalServerMessage.ParseFromString(internalServerCommand)) {
		return COMMAND_INCORRECT;
	}

	if (internalServerMessage.has_devicecommand()) {
		if(not internalServerMessage.devicecommand().has_device() || google::protobuf::util::MessageDifferencer::Equals(internalServerMessage.devicecommand().device(), device)) {
			return DEVICE_INCORRECT;
		}

		auto commandSize = internalServerMessage.devicecommand().commanddata().length();
//		auto commandData = std::make_unique<char[]>(commandSize);
//		std::memcpy(commandData.get(), internalServerMessage.devicecommand().commanddata().data(), commandSize);
		command->resize(commandSize);
		std::memcpy(&command[0], internalServerMessage.devicecommand().commanddata().data(), commandSize);
// TODO fix this

//		*command = std::string(internalServerMessage.devicecommand().commanddata().data(), commandSize);
		return OK;
	} else {
		return COMMAND_INCORRECT;
	}

}

InternalProtocol::InternalClient ProtoSerializer::createInternalConnect(const InternalProtocol::Device &device) {
	InternalProtocol::InternalClient internalClientMessage;
	InternalProtocol::DeviceConnect* deviceConnectMessage = internalClientMessage.mutable_deviceconnect();
	InternalProtocol::Device *devicePtr = deviceConnectMessage->mutable_device(); // Returns pointer to device field in deviceConnectMessage
	devicePtr->CopyFrom(device);
	return internalClientMessage;
}

int ProtoSerializer::checkAndParseConnectResponse(const std::string &internalServerConnectResponse,
												  const InternalProtocol::Device &device) {
	if(internalServerConnectResponse.empty()) {
		return NOT_OK;
	}
	InternalProtocol::InternalServer internalServerMessage;

	if (not internalServerMessage.ParseFromString(internalServerConnectResponse)) {
		return NOT_OK;
	}

	if (internalServerMessage.has_deviceconnectresponse()) {
		const auto& deviceConnectResponse = internalServerMessage.deviceconnectresponse();
		if(google::protobuf::util::MessageDifferencer::Equals(deviceConnectResponse.device(),
															  device)) {
			return DEVICE_INCORRECT;
		}

		// TODO is it okay to have this here? It is close to internal_client functionality
		switch (deviceConnectResponse.responsetype()) {
			case InternalProtocol::DeviceConnectResponse_ResponseType_OK:
				return OK;
			case InternalProtocol::DeviceConnectResponse_ResponseType_ALREADY_CONNECTED:
				return DEVICE_ALREADY_CONNECTED;
			case InternalProtocol::DeviceConnectResponse_ResponseType_MODULE_NOT_SUPPORTED:
				return MODULE_NOT_SUPPORTED;
			case InternalProtocol::DeviceConnectResponse_ResponseType_DEVICE_NOT_SUPPORTED:
				return DEVICE_TYPE_NOT_SUPPORTED;
			case InternalProtocol::DeviceConnectResponse_ResponseType_HIGHER_PRIORITY_ALREADY_CONNECTED:
				return HIGHER_PRIORITY_ALREADY_CONNECTED;
			default:
				return NOT_OK;
		}

	} else {
		return NOT_OK;
	}
}

}