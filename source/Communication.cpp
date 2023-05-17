#include <Communication.hpp>
#include <helpers/EnumMappers.hpp>

#include <ic_error_codes.h>


int Communication::sendConnectMessage(Context *context) {
	auto internalConnect = protobuf::ProtoSerializer::createInternalConnect(context->getDevice());
	auto connectMessage = protobuf::ProtoSerializer::serializeProtobufMessageToBuffer(internalConnect);
	if (context->sendMessage(connectMessage) <= 0) {
		return NOT_OK;
	}
	deallocate(&connectMessage);
	return OK;
}

int Communication::readConnectResponse(Context *context) {
	auto size = context->readSizeFromSocket();
	std::string connectResponse = context->readMessageFromSocket(size);
	if (int retCode = protobuf::ProtoSerializer::checkConnectResponse(connectResponse, context->getDevice()) != OK) {
		return retCode;
	}
	return helpers::EnumMappers::ConnectResponseToInternalClientCode(connectResponse);
}

int Communication::startReconnectSequence(Context *context, struct buffer statusMessage, uint32_t *nextMessageSize) {
	if(context->reconnect() == OK) {
		if (Communication::sendConnectMessage(context) == OK) {
			if (Communication::readConnectResponse(context) == OK) {
				if(context->sendMessage(statusMessage) <= 0) {
					return NOT_OK;
				}
				*nextMessageSize = context->readSizeFromSocket();
				if (*nextMessageSize == 0) {
					return NOT_OK;
				}
			} else {
				return UNABLE_TO_CONNECT;
			}
		} else {
			return UNABLE_TO_CONNECT;
		}
	} else {
		return UNABLE_TO_CONNECT;
	}
	return OK;
}


