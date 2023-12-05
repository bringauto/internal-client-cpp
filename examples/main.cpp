#include <internal_client.h>
#include <iostream>
#include <modules/CarAccessoryModule.pb.h>


int main() {
	struct device_identification device {2, 0, "button", "example_button", 0};
	void *context {};
	int rc = init_connection(&context, "127.0.0.1", 8888, device);
	if(rc != OK ) {
		destroy_connection(&context);
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