#include <Context.hpp>

#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <google/protobuf/util/message_differencer.h>

#include <memory_management.h>
#include <general_error_codes.h>
#include <internal_client.h>


int Context::createConnection(const char *ipv4_address, unsigned int port) {
	if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return NOT_OK; // Socket creation error
	}

	struct sockaddr_in serverAddress;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);

	if((inet_pton(AF_INET, ipv4_address, &serverAddress.sin_addr) <= 0)) {
		return NOT_OK; // Invalid address
	}
	if (connect(socket_, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		return NOT_OK; // Connection error
	}
	return OK;
}

void Context::setDevice(struct device_identification device) {
	device_.set_module(InternalProtocol::Device::Module(device.module));
	device_.set_devicerole(device.device_role);
	device_.set_devicetype(device.device_type);
	device_.set_devicename(device.device_name);
	device_.set_priority(device.priority);
}



size_t Context::sendMessage(struct buffer message) {
	if(send(socket_, &message.size_in_bytes, sizeof(uint32_t), 0) != headerSize_) {
		return NOT_OK;
	}
	size_t bytesSent = send(socket_, message.data, message.size_in_bytes,  MSG_NOSIGNAL);
	// Flag MSG_NOSIGNAL prevents the socket form sending signal SIGPIPE. This only works on Linux and must be changed for other platforms
	return bytesSent;
}

// TODO split into readSize, readCommand. In readSize i can reconnect
std::string Context::readFromSocket() {
	uint32_t commandSize;
	if (recv(socket_, &commandSize, sizeof(uint32_t), 0) != sizeof(uint32_t)) {
		return "";
	}
	auto buffer = std::make_unique<char[]>(commandSize);

	int rc = recv(socket_, buffer.get(), commandSize, 0);
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


