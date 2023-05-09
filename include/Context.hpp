#pragma once

#include <device_management.h>
#include <InternalProtocol.pb.h>
#include <memory_management.h>
#include <condition_variable>



class Context {
public:
	/**
	 * @brief Creates socket and connects it to the a server
	 *
	 * @param ipv4_address IPv4 address of the server
	 * @param port port number
	 *
	 * @return OK if connection was successfully created
	 * @return NOT_OK otherwise
	 */
	int createConnection(const char *ipv4_address, unsigned port);

	/**
	 * @brief Closes socket
	 */
	~Context();

	/**
	 * @brief Set device
	 * @param device
	 */
	void setDevice(struct device_identification device);

	/**
	 * @brief Returns current device
	 * @return device
	 */
	InternalProtocol::Device getDevice() { return device_; }

	/**
	 * @brief Reads from message from socket
	 * First reads 4 bytes, that contains the length of the message that can be read
	 * Then reads the message and saves it to a string
	 * @return string containing message
	 */
	std::string readFromSocket();

	/**
	 * @brief Sends message in buffer and return number of bytes sent
	 * First sends 4 bytes containing the message length, then the message in the buffer
	 * @param message buffer containing message that will be sent
	 * @return number of bytes sent
	 */
	size_t sendMessage(struct buffer message);

	/**
	 * @brief Save commandData
	 * @param command string containing command data
	 */
	void saveCommand(const std::string &command);

	/**
	 * @brief Returns pointer to the command data
	 * To get the length of the command data, use getCommandSize() function
	 * @return pointer to char
	 */
	char* getCommandData() const { return commandData_.get(); };

	/**
	 * @brief Return length of the command data
	 * @return Data size
	 */
	size_t getCommandSize() const { return commandSize_; };

private:
	/// Socket connected to the server
	int socket_ { -1};
	/// Protobuf Device message
	InternalProtocol::Device device_ {};
	/// Command data pointer
	std::unique_ptr<char[]> commandData_ {};
	/// Size of commandData_
	size_t commandSize_ { 0 };

	/// Size of message header, where the length of the message is set
	static constexpr short headerSize_ { 4 };
};
