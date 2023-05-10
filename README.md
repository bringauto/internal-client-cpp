# Internal Client

Implementation of Internal client library for fleet protocol v2 as described
[here](https://docs.google.com/document/d/19h2yEh3DzIizKZc-iHWpUrQIrLyop7FQUBzUi_K9LWY/view).  
API of this implementation is based on ANSI C api proposed in fleet protocol internal client [header file](https://gitlab.bringauto.com/bring-auto/fleet-protocol-v2/fleet-protocol/-/blob/master/lib/module_api/include/internal_client/internal_client.h).


## Requirements

- CMLib
- CMake [>= 3.25] 

## Usage
>Example usage can be seen in `examples` directory.


### Client initialization
First you need to initialize device and context:
```c++
struct device_identification device { <module_num>, <device_type>, <device_role>, <device_name>, <priority>};
void *context {};
```
Then create connection to an Internal server:
```c++
init_connection(&context, "127.0.0.1", 8888, device)
```

The return code must be checked to find out, if an error occurred or the program can continue.
If the connection was successful, context will be initialized.

### Sending and receiving data
After connection was established with server, device status can be sent. This is done by using `send_status` method:
```c++
send_status(context, binary_payload, 30)
```
Client will try to send status with provided binary payload to server, then will wait for a command.

The timeout given to send_status is the maximum time for sending and for receiving, therefore the maximum time this function can take is `2*timeout`.

If no error occurs during the communication and a response with a command is received, it can be obtained using:
```c++
get_command(context, &command_buffer)
```
The command_buffer must be initialized before use. 
1. If the data pointer is not allocated, it will allocate it.
2. If the data pointer is allocated, but only for not sufficient size. The pointer will be reallocated
3. Command data will be copied into command_buffer data pointer and the size_in_bytes will be set accordingly

If the client was disconnected by the server, it will attempt to reconnect once.

### Destroying client
After client is no longer needed, or an error occurred, it needs to be destroyed using:
```c++
destroy_connection(&context)
```

## CMake arguments
* BRINGAUTO_SAMPLES - configure examples
* BRINGAUTO_INSTALL - enable install
* BRINGAUTO_PACKAGE - Enable package generation
* BRINGAUTO_SYSTEM_DEP - Enable system dependencies, so they won't be installed with CMLib
  * If this option is used, all library requirements must be installed manually
  * Library requirements:
    * protobuf [= 3.21.12]
  
## Install
To install, use the following commands:
```shell
$ mkdir _build && cd _build
$ cmake -DBRINGAUTO_INSTALL=ON -DCMLIB_DIR=<path_to_cmlib> ..
$ make install
```

## Package
To generate packages, use the following commands:
```shell
$ mkdir _build && cd _build
$ cmake -DBRINGAUTO_INSTALL=ON -DBRINGAUTO_PACKAGE=ON -DCMLIB_DIR=<path_to_cmlib> ..
$ cpack
```

## Include to project

TODO