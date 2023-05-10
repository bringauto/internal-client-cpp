#pragma once

#include <Context.hpp>
#include <protobuf/ProtoSerializer.hpp>

class Communication {
public:
	static int sendConnectMessage(Context *context);

	static int readConnectResponse(Context *context);

	static int startReconnectSequence(Context *context, struct buffer statusMessage, uint32_t *nextMessageSize);
};
