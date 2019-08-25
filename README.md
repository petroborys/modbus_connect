# Modbus CLI

This package provides a command line interface to send Modbus requests. It supports both Modbus RTU and Modbus TCP modes. It depends on the excellent library [libmodbus](http://libmodbus.org/) for C.

Once installed, it is possible to check its options and usage examples by executing `modbus-cli -h`:

```
Usage: modbus-cli [options...]
Examples:
	 modbus-cli -r -d /dev/ttyS1 -b 9600 -f 4 -s 1 -a 0 -n 20
	 modbus-cli -t -i 192.168.1.1 -p 502 -f 4 -s 1 -a 0 -n 20
Options: (R) means Modbus RTU, (T) means Modbus TCP
 -r,	 Establish a Modbus RTU connection
 -t,	 Establish a Modbus TCP connection
 -d,	 Define the device, default is /dev/ttyS1 (R)
 -b,	 Define the baudrate, default is 9600 (R)
 -i,	 Define the IP address (T)
 -p,	 Define the port (T)
 -f,	 Define the function code.
		 3=Read Holding Registers
		 4=Read Input Registers
		 5=Write Single Coil
		 6=Write Single Register
 -s,	 Define the slave ID to send the Modbus request to
 -a,	 Define the start address
 -n,	 Define the number of registers (for -f = 3 or 4)
 -v,	 Define the value to write TRUE or FALSE for (for -f = 5 or 6)
```

The package only contains three files:
* `src/modbus-cli.c`: it is the source code of the program
* `src/makefile`: it is the makefile to compile the program itself. `libmodbus` is linked from this file.
* `Makefile`: it is the makefile to compile the package. `libmodbus` is listed as a dependency here so it will be cross-compiled as well so it can be installed later in the router.