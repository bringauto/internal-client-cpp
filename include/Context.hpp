#pragma once

#include <device_management.h>
#include <InternalProtocol.pb.h>
#include <memory_management.h>
#include <condition_variable>



class Context {
public:

	int createConnection(const char *ipv4_address, unsigned port);

	~Context();

	void setDevice(struct device_identification device);

	InternalProtocol::Device getDevice() { return device_; }

	std::string readFromSocket();

	/**
	 * @brief Sends message in buffer and return number of bytes sent
	 * @param message buffer containing message that will be sent
	 * @return number of bytes sent
	 */
	size_t sendMessage(struct buffer message);

	void saveCommand(const std::string &command);

	char* getCommandData() { return commandData_.get(); };

	size_t getCommandSize() {return commandSize_; };

private:
	int socket_ { -1};
	InternalProtocol::Device device_ {};
	std::unique_ptr<char[]> commandData_ {};
	size_t commandSize_;
};
