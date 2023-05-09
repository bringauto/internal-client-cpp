#include <internal_client.h>
#include <Context.hpp>
#include <protobuf/ProtoSerializer.hpp>
#include <helpers/EnumMappers.hpp>

#include <thread>
#include <future>



int init_connection(void **context, const char *const ipv4_address, unsigned port,
					const struct device_identification device) {
	auto newContext = new Context();

	if(newContext->createConnection(ipv4_address, port) != OK) {
		return UNABLE_TO_CONNECT;
	}

	*context = newContext;

	newContext->setDevice(device);
	auto internalConnect = protobuf::ProtoSerializer::createInternalConnect(newContext->getDevice());
	auto connectMessage = protobuf::ProtoSerializer::serializeProtobufMessageToBuffer(internalConnect);
	if (newContext->sendMessage(connectMessage) <= 0) {
		return NOT_OK;
	}
	clear_buffer(&connectMessage);
	std::string connectResponse = newContext->readFromSocket();
	if (int retCode = protobuf::ProtoSerializer::checkConnectResponse(connectResponse, newContext->getDevice()) != OK) {
		return retCode;
	}
	return helpers::EnumMappers::ConnectResponseToInternalClientCode(connectResponse);
}

int destroy_connection(void **context) {
	if(*context == nullptr) {
		return NOT_OK;
	}
	auto currentContext = (Context **)context;
	delete *currentContext;
	*currentContext = nullptr;
	return OK;
}

int send_status(void *context, const struct buffer status, unsigned timeout) {
	if(context == nullptr) {
		return CONTEXT_INCORRECT;
	}
	auto currentContext = (Context *)context;

	auto internalClientMessage = protobuf::ProtoSerializer::createInternalStatus(status,
																				currentContext->getDevice());
	struct buffer statusMessage = protobuf::ProtoSerializer::serializeProtobufMessageToBuffer(internalClientMessage);

	auto rc = OK;
	auto threadStatus = std::async(std::launch::async, [&]() {
		if(currentContext->sendMessage(statusMessage) <= 0) {
			rc = NOT_OK;
		}
	}).wait_for(std::chrono::seconds(timeout));
	clear_buffer(&statusMessage);

	if(threadStatus == std::future_status::timeout) {
		return TIMEOUT_OCCURRED;
	} else if(rc == NOT_OK) {
		return NOT_OK;
	}

	std::string internalServerMessageString {};
	threadStatus = std::async(std::launch::async, [&]() {
		internalServerMessageString = currentContext->readFromSocket();
	}).wait_for(std::chrono::seconds(timeout));

	if(threadStatus == std::future_status::timeout) {
		return TIMEOUT_OCCURRED;
	}

	std::string command {};
	if(protobuf::ProtoSerializer::checkAndParseCommand(&command, internalServerMessageString, currentContext->getDevice()) == NOT_OK) {
		return COMMAND_INCORRECT;
	}
	currentContext->saveCommand(command);
	return OK;
}

int get_command(void *context, struct buffer *command) {
	if(context == nullptr) {
		return CONTEXT_INCORRECT;
	}
	auto currentContext = (Context *)context;

	auto newCommandSize = currentContext->getCommandSize();
	if(command->size_in_bytes <= 0) {
		return NO_COMMAND_AVAILABLE;
	}

	if (command->size_in_bytes == 0) {
		if ((command->data = malloc(newCommandSize)) == nullptr) {
			return NOT_OK;
		}
	} else if (command->size_in_bytes > 0 && command->data == nullptr) {
		return NOT_OK;
	} else if (command->size_in_bytes < newCommandSize) {
		if ((command->data = realloc(command->data, newCommandSize)) == nullptr) {
			return NOT_OK;
		}
	}
	std::memcpy(command->data, currentContext->getCommandData(), newCommandSize);
	command->size_in_bytes = newCommandSize;
	return OK;
}