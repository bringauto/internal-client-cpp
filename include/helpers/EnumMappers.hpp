#pragma once

#include <string>


namespace helpers {

class EnumMappers {
public:
	/**
	 * @brief Gets response_type from Connect response and maps it to an Enum described in ic_error_codes
	 *
	 * @param internalServerConnectResponse Serialized Connect Response message
	 * @return OK, if connect response is OK
	 * @return Error code according to ic_error_codes
	 */
	static int ConnectResponseToInternalClientCode(const std::string &internalServerConnectResponse);
};
}