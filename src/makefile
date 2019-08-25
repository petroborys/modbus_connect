# build modbus_tcp executable when user executes "make"
LIBS=-lmodbus

modbus_connect: main.o modbus_connect.o
	$(CC) $(LDFLAGS) $(LIBS) main.o modbus_connect.o -o modbus_connect

main.o: main.c
	$(CC) -c $(CFLAGS) main.c

modbus_connect.o: modbus_connect.c modbus_connect.h
	$(CC) -c $(CFLAGS) modbus_connect.c

# remove object files and executable when user executes "make clean"
clean:
	rm *.o modbus_connect