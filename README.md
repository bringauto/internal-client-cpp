# Internal Client
TODO

Proof of concept implementation of Internal client library for fleet protocol v2 as described
[here](https://docs.google.com/document/d/19h2yEh3DzIizKZc-iHWpUrQIrLyop7FQUBzUi_K9LWY/edit).  
API of this implementation is based on ANSI C api proposed in fleet protocol internal client [header file](https://gitlab.bringauto.com/bring-auto/fleet-protocol-v2/fleet-protocol/-/blob/master/lib/module_api/include/internal_client/internal_client.h). Some things are adapted to python ways of doing things, e.g. errors are handled via exceptions instead of return values.

## Requirements

- `protobuf`

To install requirements defined in `requirements.txt` use 
```
pip install -r requirements.txt
```

## Usage
>Example usage can be seen in `example.py` file.

Internal client library provides implementations of clients for different modules. See [adding support for new module](#adding-support-for-modules).  
_E.g._ to use client of CarAccessory module import it from internal_client:
```python
from internal_client import CarAccessoryInternalClient
```
Every method of client can also raise [exceptions](internal_client/client_lib/exceptions.py) defined in internal client library. To catch these exceptions, it is needed to import them:
```python
from internal_client import exceptions
```
### Client initialization
First you need to initialize client with information about device and desired internal server:
```python
client = CarAccessoryInternalClient("127.0.0.1", 8888, "button1", 0, "left_button", 0)
```
Two types of exceptions can be raised:
1. any of `CommunicationExceptions` indicating problem with connection to server on transport layer
2. any of `ConnectExceptions` indicating that server refused to connect device based on response in fleet protocol (e.g. `DeviceNotSupported`)

If no exception was raised during initialization, client is now connected to internal server and ready to be used.

### Sending and receiving data
After connection was established with server, device status can be sent. This is done by using `send_status` method:
```python
client.send_status(binary_payload, timeout=10)
```
Client will try to send status with provided binary payload to server. If no error occurs during communication and command response is received, it can be obtained using:
```python
binary_command = client.get_command()
```
which returns binary payload of command. 
If error occurs during communication, client will try to reestablish connection with server and try again. If status was not sent even after reconnection, client connection will be destroyed and one of these exceptions will be raised from `send_status`:
1. any of `CommunicationExceptions` indicating problem with connection to server on transport layer even after reconnection
2. any of `ConnectExceptions` indicating that during reconnection server refused to connect device based on response in fleet protocol (e.g. `ModuleNotSupported`)

After raising exception, client is destroyed and needs to be [recreated](#client-initialization) to communicate with internal server. 

### Destroying client
After client is no longer needed, it needs to be destroyed using:
```python
client.destroy()
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
