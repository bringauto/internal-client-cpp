# Internal Client

Implementation of Internal client library for fleet protocol v2 as described
[here](https://docs.google.com/document/d/19h2yEh3DzIizKZc-iHWpUrQIrLyop7FQUBzUi_K9LWY/view).  
API of this implementation is based on ANSI C api proposed in fleet protocol internal client [header file](https://gitlab.bringauto.com/bring-auto/fleet-protocol-v2/fleet-protocol/-/blob/master/lib/module_api/include/internal_client/internal_client.h).


## Requirements

- `CMLib`
- `CMake` >= 3.25 

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
Client will try to send status with provided binary payload to server.
If no error occurs during communication and a response with command is received, it can be obtained using:
```c++
get_command(context, &command_buffer)
```
The command_buffer must be initialized before use. 
1. If the data pointer is not allocated, it will allocate it.
2. If the data pointer is allocated, but only for not sufficient size. The pointer will be reallocated
3. Command data will be copied into command_buffer data pointer and the size_in_bytes will be set accordingly

### Destroying client
After client is no longer needed, it needs to be destroyed using:
```c++
destroy_connection(&context)
```
>**Note**: If exception was raised during initialization or sending status, client is destroyed automatically.

## Adding support for modules
Every client is subclass of base client defined in [`InternalClient.py`](internal_client/client_lib/InternalClient.py). Module specific clients are then defined in [internal_client](internal_client/) directory. To add support for new module, follow these steps:
1. Create client file in [internal_client](internal_client/) directory. 
2. In this file, import `InternalClient` from `.client_lib`
3. Create new client class as subclass of `InternalClient`. Consider using proper name, such as `{MODULE_NAME}InternalClient`, where `MODULE_NAME` is name of desired module in CapitalizedWords (e.g. `CarAccessory`).
4. In this newly created class, specify class variable `MODULE_ID` with integer value representing value of module id.
5. Add import of this class into [`__init__.py`](internal_client/__init__.py) to allow for better usage of this newly created module client. It is also a good idea to add name of this class into `__all__` dunder tuple for cases when some lunatic uses `import *`.
### Module client example
[`_mission_client.py`](internal_client/_mission_client.py)
```python
from .client_lib import InternalClient
class MissionInternalClient(InternalClient):
    MODULE_ID = 1

```
[definition in `__init__.py`](internal_client/__init__.py)
```python
__all__ = (
    "exceptions",
    "CarAccessoryInternalClient",
    "MissionInternalClient" # new client
)
# new client
from ._mission_client import MissionInternalClient

from ._car_accessory_client import CarAccessoryInternalClient
from .client_lib import exceptions
```
