#include <fleet_protocol/common_headers/memory_management.h>
#include <fleet_protocol/internal_client/internal_client.h>

#include <nlohmann/json.hpp>

#include <cstring>
#include <iostream>

/**
 * @brief Handles the return code and cleans up the connection context if necessary.
 *
 * This function takes a return code `rc` and a pointer to a connection context `context`.
 * If the return code is not `OK`, it destroys the connection and sets the context pointer to `nullptr`.
 *
 * @param rc The return code to handle.
 * @param context A pointer to the connection context.
 * @return The return code `rc` if it is `OK`, otherwise the same return code and the connection context is cleaned up.
 */
int handleRc(int rc, void **context)
{
    if (rc != OK)
    {
        if (*context)
        {
            destroy_connection(context);
            *context = nullptr;
        }
        return rc;
    }
    return OK;
}

/**
 * @brief Connects to the internal client.
 *
 * This function establishes a connection to the internal client using the provided device role, device name, IP
 * address, and port number.
 *
 * @param context A pointer to a void pointer that will hold the context of the connection.
 * @param deviceRole The role of the device.
 * @param deviceName The name of the device.
 * @param ip The IP address of the module gateway.
 * @param port The port number of the module gateway.
 * @return An integer representing the result of the connection. A value of 0 indicates success, while a negative value
 * indicates an error.
 */
int connectToInternalClient(void **context, const std::string &deviceRole, const std::string &deviceName,
                            const std::string &ip, unsigned port)
{
    buffer deviceRoleBuffer{};
    allocate(&deviceRoleBuffer, deviceRole.length());
    std::memcpy(deviceRoleBuffer.data, deviceRole.c_str(), deviceRoleBuffer.size_in_bytes);
    buffer deviceNameBuffer{};
    allocate(&deviceNameBuffer, deviceName.length());
    std::memcpy(deviceNameBuffer.data, deviceName.c_str(), deviceNameBuffer.size_in_bytes);

    /**
     * @struct device_identification
     * @brief Represents the identification information of a device.
     *
     * This struct contains the following parameters:
     * - `moduleNumber`: The number of the module that communicates with the device. In this case, an IO module with
     * the number `2` is used.
     * - `deviceTypeNumber`: The number representing the device type. In this case, a button with the number
     * `4` is used.
     * - `deviceNameBuffer`: A buffer that holds the name of the device.
     * - `deviceRoleBuffer`: A buffer that holds the role of the device.
     * - `devicePriority`: An integer representing the device's priority, with a default value of `0`.
     */
    struct device_identification device
    {
        2, 4, deviceNameBuffer, deviceRoleBuffer, 0
    };

    int rc = init_connection(context, ip.c_str(), port, device);

    deallocate(&deviceRoleBuffer);
    deallocate(&deviceNameBuffer);
    return rc;
}

/**
 * Sends the status to the specified context.
 *
 * @param context A pointer to the context.
 * @param message The status message to send.
 * @return The return code indicating the success or failure of the operation.
 */
int sendStatus(void *context, bool message)
{
    // Structure of the command json message is dependent on the device type
    nlohmann::json jsonArray = nlohmann::json::array();
    nlohmann::json inArray = {{{"inNum", 1}, {"inSt", message}}};
    jsonArray.push_back(inArray);
    jsonArray.push_back(nlohmann::json::array());
    jsonArray.push_back({{"butPr", 0}});
    std::string statusMessage = jsonArray.dump();
    // std::cout << "statusMessage: " << statusMessage << std::endl;

    buffer status{};
    allocate(&status, statusMessage.length());
    std::memcpy(status.data, statusMessage.c_str(), status.size_in_bytes);
    int rc = send_status(context, status, 60);
    deallocate(&status);
    return rc;
}

/**
 * @brief Retrieves a command from the given context and assigns it to the provided commandString.
 *
 * This function retrieves a command from the specified context and assigns it to the provided commandString.
 * The command is stored in a buffer and then converted to a string before being assigned.
 *
 * @param context A pointer to the context from which to retrieve the command.
 * @param commandString A reference to a string where the retrieved command will be assigned.
 * @return The return code indicating the success or failure of the operation.
 */
int getCommand(void *context, std::string &commandString)
{
    buffer command{};
    int rc = get_command(context, &command);
    commandString.assign(static_cast<char *>(command.data), command.size_in_bytes);
    deallocate(&command);
    return rc;
}

int main()
{
    void *context{};
    int rc = connectToInternalClient(&context, "example_button", "button", "127.0.0.1", 1636);
    if (handleRc(rc, &context) != OK)
    {
        return rc;
    }

    rc = sendStatus(context, false);
    if (handleRc(rc, &context) != OK)
    {
        return rc;
    }

    // Before obtaining the command, the status message must be always sent.
    std::string commandString;
    rc = getCommand(context, commandString);
    if (handleRc(rc, &context) != OK)
    {
        return rc;
    }
    // The module gateway may return a "null" string if no command is available.
    std::cout << "received command: " << commandString << std::endl;

    return 0;
}
