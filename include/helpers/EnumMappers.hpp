#pragma once

#include <string>


namespace helpers {

class EnumMappers {
public:
	static int ConnectResponseToInternalClientCode(const std::string &internalServerConnectResponse);
};
}