# build modbus_tcp executable when user executes "make"
LIBS=-lmodbus -lmosquitto

modbus_connect: main.o modbus_connect.o mqtt_connect.o
	$(CC) $(LDFLAGS) $(LIBS) main.o modbus_connect.o mqtt_connect.o -o modbus_connect

main.o: main.c
	$(CC) -c $(CFLAGS) main.c

modbus_connect.o: modbus_connect.c modbus_connect.h
	$(CC) -c $(CFLAGS) modbus_connect.c

mqtt_connect.o: mqtt_connect.c mqtt_connect.h
	$(CC) -c $(CFLAGS) mqtt_connect.c

mkjson.o: mkjson.c mkjson.h
	$(CC) -c $(CFLAGS) mkjson.c

# remove object files and executable when user executes "make clean"
clean:
	rm *.o modbus_connect