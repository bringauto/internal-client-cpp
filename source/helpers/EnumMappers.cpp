#include <helpers/EnumMappers.hpp>

#include <InternalProtocol.pb.h>
#include <fleet_protocol/internal_client/error_codes.h>
#include <fleet_protocol/common_headers/general_error_codes.h>


namespace helpers {

int helpers::EnumMappers::ConnectResponseToInternalClientCode(const std::string &internalServerConnectResponse) {
	InternalProtocol::InternalServer internalServerMessage;
	internalServerMessage.ParseFromString(internalServerConnectResponse);

	switch(internalServerMessage.deviceconnectresponse().responsetype()) {
		case InternalProtocol::DeviceConnectResponse_ResponseType_OK:
			return OK;
		case InternalProtocol::DeviceConnectResponse_ResponseType_ALREADY_CONNECTED:
			return DEVICE_ALREADY_CONNECTED;
		case InternalProtocol::DeviceConnectResponse_ResponseType_MODULE_NOT_SUPPORTED:
			return MODULE_NOT_SUPPORTED;
		case InternalProtocol::DeviceConnectResponse_ResponseType_DEVICE_NOT_SUPPORTED:
			return DEVICE_TYPE_NOT_SUPPORTED;
		case InternalProtocol::DeviceConnectResponse_ResponseType_HIGHER_PRIORITY_ALREADY_CONNECTED:
			return HIGHER_PRIORITY_ALREADY_CONNECTED;
		default:
			return NOT_OK;
	}
}

}