#include <fleet_protocol/internal_client/internal_client.h>
#include <fleet_protocol/common_headers/memory_management.h>
#include <iostream>
#include <cstring>


int main() {

	std::string deviceRole = "example_button";
	std::string deviceName = "button";
	buffer deviceRoleBuffer {};
	allocate(&deviceRoleBuffer, deviceRole.length());
	std::memcpy(deviceRoleBuffer.data, deviceRole.c_str(), deviceRoleBuffer.size_in_bytes);
	buffer deviceNameBuffer {};
	allocate(&deviceNameBuffer, deviceName.length());
	std::memcpy(deviceNameBuffer.data, deviceName.c_str(), deviceNameBuffer.size_in_bytes);

	struct device_identification device {2, 0, deviceNameBuffer, deviceRoleBuffer, 0};
	void *context {};
	int rc = init_connection(&context, "127.0.0.1", 8888, device);
	deallocate(&deviceRoleBuffer);
	deallocate(&deviceNameBuffer);
	if(rc != OK ) {
		if(context != nullptr) {
			destroy_connection(&context);
		}
		return rc;
	}

	std::string statusData = "{\"pressed\": true}";
	buffer status {};
	allocate(&status, statusData.length());
	std::memcpy(status.data, statusData.c_str(), status.size_in_bytes);

	rc = send_status(context, status, 60);
	if(rc != OK) {
		destroy_connection(&context);
		return rc;
	}


	deallocate(&status);

	buffer command {};
	rc = get_command(context, &command);
	if(rc != OK ) {
		destroy_connection(&context);
		return 1;
	}

	std::string commandString;
	commandString.assign(static_cast<char*>(command.data), command.size_in_bytes);
	std::cout << "Command = " << commandString << std::endl;

	deallocate(&command);

	destroy_connection(&context);
	return 0;
}