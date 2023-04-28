
#include <internal_client.h>
#include <iostream>
#include <Context.hpp>
#include <protobuf/ProtoSerializer.hpp>
#include <modules/CarAccessoryModule.pb.h>


/// Used for debuging during developmnet
int main() {
	struct device_identification device {2, 0, "test", "test1", 0};
	void *context {};
	int rc = init_connection(&context, "127.0.0.1", 8888, device);
	if(rc != OK ) {
		return 1;
	}
	auto con = (Context*)context;

	CarAccessoryModule::ButtonStatus buttonStatus;
	buttonStatus.set_ispressed(false);

	auto status = protobuf::ProtoSerializer::serializeProtobufMessage(buttonStatus);

	for(int i = 0; i < 10; ++i) {
		rc = send_status(con, status, 60);
		if(rc != OK) {
			return 1;
		}
	}
	buffer command {};
	rc = get_command(con, &command);
	if(rc != OK ) {
		return 1;
	}

	CarAccessoryModule::ButtonCommand commandProto;
	commandProto.ParseFromArray(command.data, command.size_in_bytes);
	std::cout << commandProto.command() << std::endl;
}