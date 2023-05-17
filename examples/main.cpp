#include <internal_client.h>
#include <iostream>
#include <protobuf/ProtoSerializer.hpp>
#include <modules/CarAccessoryModule.pb.h>


int main() {
	struct device_identification device {2, 0, "test", "test1", 0};
	void *context {};
	int rc = init_connection(&context, "127.0.0.1", 8888, device);
	if(rc != OK ) {
		destroy_connection(&context);
		return rc;
	}

	CarAccessoryModule::ButtonStatus buttonStatus;
	buttonStatus.set_ispressed(true);

	auto status = protobuf::ProtoSerializer::serializeProtobufMessageToBuffer(buttonStatus);

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

	CarAccessoryModule::ButtonCommand commandProto;
	commandProto.ParseFromArray(command.data, command.size_in_bytes);
	std::cout << commandProto.command() << std::endl;

	deallocate(&command);

	destroy_connection(&context);

}