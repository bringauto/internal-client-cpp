#pragma once

#include <device_management.h>
#include <memory_management.h>
#include <InternalProtocol.pb.h>

#include <condition_variable>
#include <netinet/in.h>



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
	 * @brief Reconnects socket to server
	 *
	 * @return OK if reconnect was successful
	 * @return NOT_OK if and error occurred
	 */
	int reconnect();

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
	 * @brief Reads 4 bytes, that contains the length of a message that follows it in socket
	 *
	 * @return size in bytes of the next message
	 */
	uint32_t readSizeFromSocket() const;

	/**
	 * @brief Reads from message from socket
	 * Reads number of bytes given by parameter from socket and saves it to a string
	 *
	 * @param commandSize number of bytes to read
	 * @return string containing message
	 */
	std::string readCommandFromSocket(uint32_t commandSize) const;

	/**
	 * @brief Sends message in buffer and return number of bytes sent
	 * First sends 4 bytes containing the message length, then the message in the buffer
	 * @param message buffer containing message that will be sent
	 * @return number of bytes sent
	 */
	size_t sendMessage(struct buffer message) const;

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

	/**
	 * @brief Closes socket
	 */
	~Context();

private:
	/// Socket connected to the server
	int socket_ { -1};
	/// Server address saved for reconnecting
	struct sockaddr_in serverAddress_;
	/// Protobuf Device message
	InternalProtocol::Device device_ {};
	/// Command data pointer
	std::unique_ptr<char[]> commandData_ {};
	/// Size of commandData_
	size_t commandSize_ { 0 };

	/// Size of message header, where the length of the message is set
	static constexpr short headerSize_ { 4 };
};
