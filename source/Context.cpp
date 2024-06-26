#include <Context.hpp>

#include <fleet_protocol/common_headers/memory_management.h>
#include <fleet_protocol/internal_client/error_codes.h>
#include <fleet_protocol/internal_client/internal_client.h>
#include <google/protobuf/util/message_differencer.h>

#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


int Context::createConnection(const char *ipv4_address, unsigned int port) {
	if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return NOT_OK; // Socket creation error
	}

	serverAddress_.sin_family = AF_INET;
	serverAddress_.sin_port = htons(port);

	if((inet_pton(AF_INET, ipv4_address, &serverAddress_.sin_addr) <= 0)) {
		return NOT_OK; // Invalid address
	}
	if (connect(socket_, (struct sockaddr*)&serverAddress_, sizeof(serverAddress_)) < 0) {
		return NOT_OK; // Connection error
	}
	return OK;
}

int Context::reconnect() {
	close(socket_);
	if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return NOT_OK; // Socket creation error
	}
	if (connect(socket_, (struct sockaddr*)&serverAddress_, sizeof(serverAddress_)) < 0) {
		return NOT_OK; // Connection error
	}
	return OK;
}

void Context::setDevice(struct device_identification device) {
	device_.set_module(InternalProtocol::Device::Module(device.module));
	device_.set_devicerole(std::string(static_cast<const char *>(device.device_role.data), device.device_role.size_in_bytes));
	device_.set_devicetype(device.device_type);
	device_.set_devicename(std::string(static_cast<const char *>(device.device_name.data), device.device_name.size_in_bytes));
	device_.set_priority(device.priority);
}



size_t Context::sendMessage(struct buffer message) const {
	if(send(socket_, &message.size_in_bytes, sizeof(uint32_t), 0) != headerSize_) {
		return 0;
	}
	size_t bytesSent = send(socket_, message.data, message.size_in_bytes,  MSG_NOSIGNAL);
	/// Flag MSG_NOSIGNAL prevents the socket form sending signal SIGPIPE. This only works on Unix and must be changed for other platforms
	return bytesSent;
}

uint32_t Context::readSizeFromSocket() const {
	uint32_t commandSize;
	if (recv(socket_, &commandSize, sizeof(uint32_t), 0) != sizeof(uint32_t)) {
		return 0;
	}
	return commandSize;
}

std::string Context::readMessageFromSocket(uint32_t commandSize) const {
	auto buffer = std::make_unique<char[]>(commandSize);

	ssize_t rc = recv(socket_, buffer.get(), commandSize, 0);
	if (rc == -1) {
		return "";
	}
	std::string ret(buffer.get(), commandSize);
	return ret;
}

void Context::saveCommand(const std::string &command) {
	commandSize_ = command.length();
	commandData_ = std::make_unique<char[]>(commandSize_);
	std::memcpy(getCommandData(), command.data(), commandSize_);
}

Context::~Context() {
	close(socket_);
}





