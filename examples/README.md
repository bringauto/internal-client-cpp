# Internal Client example
Example usage of the Internal Client library

## Build
In project root directory:
```shell
$ mkdir _build && cd _build
$ cmake -DBRINGAUTO_SAMPLES=ON -DCMLIB_DIR=<path_to_cmlib> ..
$ make 
```

## Run
For this example to work, module gateway must be started first. Ip address is set to default localhost.
```shell
$ cd _build
$ ./example
```