#pragma once

#include <memory_management.h>
#include <InternalProtocol.pb.h>

namespace protobuf {

/**
 * @brief Class for serializing, deserializing and checking protobuf messages
 */
class ProtoSerializer {
public:
	/**
	 * @brief Serialize any Protobuf message into buffer structure
	 *
	 * Buffer must be freed after use!
	 *
	 * @param protobufMessage Protobuf message
	 * @return buffer structure with allocated data pointer
	 */
	static struct buffer serializeProtobufMessageToBuffer(const google::protobuf::Message &protobufMessage);

	/**
	 * @brief Combines serialized status data and device into Internal Client status message
	 *
	 * @param statusData status data serialized in buffer
	 * @param device protobuf Device message containing current device
	 *
	 * @return Internal Client message containing Status
	 */
	static InternalProtocol::InternalClient
	createInternalStatus(const struct buffer &statusData, const InternalProtocol::Device &device);

	/**
	 * @brief Creates Internal Client connect message
	 *
	 * @param device protobuf Device message containing current device
	 *
	 * @return Internal Client message containing DeviceConnect
	 */
	static InternalProtocol::InternalClient createInternalConnect(const InternalProtocol::Device &device);

	/**
	 * @brief Check if Internal Server message contains ConnectResponse and if is targeted for current device
	 *
	 * @param internalServerConnectResponse message received from Internal Server after sending Connect message
	 * @param device protobuf Device message containing current device
	 *
	 * @return OK if target device is same as current and connect response can be parsed from the message
	 * @return DEVICE_INCORRECT if target device is different than current device
	 * @return NOT_OK if message cannot be parsed or doesn't contain connect response
	 */
	static int checkConnectResponse(const std::string &internalServerConnectResponse, const InternalProtocol::Device& device);

	/**
	 * @brief Check if Internal Server message contains Device Command and if is targeted for current device
	 * If the Internal Server message is correct, Parses command data into command string.
	 *
	 * @param command pointer to string, to which command data will be copied
	 * @param internalServerCommand message received from Internal Server after sending Status message
	 * @param device protobuf Device message containing current device
	 *
	 * @return OK if target device is same as current and command was parsed into input string
	 * @return DEVICE_INCORRECT if target device is different than current device
	 * @return COMMAND_INCORRECT if message cannot be parsed or doesn't contain Device Command
	 * @return NOT_OK if message is empty
	 */
	static int checkAndParseCommand(std::string &command, const std::string &internalServerCommand,
									const InternalProtocol::Device& device);

};

}