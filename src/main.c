
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "modbus_connect.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#include "mkjson.h"
#include "mqtt_connect.h"

// modbus_connect begin
#include "modbus_connect.h"
// modbus_connect end

#define DELAY_SEND_TIME 5 //sec
// #define PORT 10333
#define PORT_SSL 20333

bool send_mqtt_1m = false;

typedef struct DataStruct
{
	int temp;
	time_t time;
} t_data;

t_data data;

struct mqttServerSetting mqttSetting; 

void timer_handler_1m()
{
	send_mqtt_1m = true;
}

void timer_init()
{
	struct sigaction psa;
	struct itimerval tv;

	memset(&psa, 0, sizeof(psa));
	psa.sa_handler = &timer_handler_1m;
	sigaction(SIGALRM, &psa, NULL);


	tv.it_interval.tv_sec = DELAY_SEND_TIME;
	tv.it_interval.tv_usec = 0;
	tv.it_value.tv_sec = DELAY_SEND_TIME;
	tv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &tv, NULL);
}

int send_data(t_data data)
{
	char *buf = malloc(64);
	// sprintf(buf,"\nUnix time: %d",data.time);
	// int rc = mqtt_send(buf, "temp");

	// char *json  = mkjson( MKJSON_OBJ, 2,
	// 		MKJSON_INT, "temp", data.temp,
	// 		MKJSON_LLINT, "time", data.time
	// 		);

	//Temporary solution
	sprintf (buf, "{\"temp\": %d, \"time\": %d}", data.temp, data.time);
	printf("%s\n", buf);


	int rc = mqtt_send(buf, "temp");

	// char *json = mkjson( MKJSON_OBJ, 2,
	// 		MKJSON_INT, "temp", data.temp,
	// 		MKJSON_LLINT, "time", data.time
	// 		);
	// printf("%s\n", json);
	// int rc = mqtt_send(json, "temp");
		
	return rc;
}

int main(int argc, char *argv[])
{
	// modbus_connect begin
	ModbusError           status;
	ModbusClientsList     clientsList;
	ModbusClientsDataList clientsDataList;
	// modbus_connect end

	printf("\r\nMosquitto test SSL\r\n");

	timer_init();

	mqttSetting.host = "m15.cloudmqtt.com";
	mqttSetting.port = PORT_SSL;
	mqttSetting.login = "thmcoslv";
	mqttSetting.passwd = "odUivT2WEIsW";
	mqttSetting.ssl_crt = "/etc/ssl/certs/ca-certificates.crt";

	// modbus_connect begin
	clientsList.clientsCnt = 1;               // One client
	clientsList.clients[0].id = 1;            // Client id
	clientsList.clients[0].port = 502;        // Port
	clientsList.clients[0].offset = 0;        // Bytes offset
	clientsList.clients[0].numOfBytes = 1;    // Bytes num
	snprintf(clientsList.clients[0].ipAdress, IP_BUF_SIZE, "%s", "192.168.1.226"); // Ip

	status = modbusInit(&clientsList);

	if(status == MBE_OK)
	{
		fprintf(stdout, "Modbus: Init OK\n");
	}
	else if(status == MBE_NOT_ALL)
	{
		fprintf(stdout, "Modbus: Not all clients inited\n");
	}
	else
	{
		fprintf(stdout, "Modbus: Init error\n");
		modbusDeinit();
	}
	// modbus_connect end

	mqtt_init(mqttSetting);

	while(1) {

		if (send_mqtt_1m) {

			// modbus_connect begin
			// Receive data
			status = modbusReceiveData(&clientsDataList);

			if(status == MBE_OK)
			{
				fprintf(stdout, "Modbus: Receive OK\n");
			}
			else if(status == MBE_NOT_ALL)
			{
				fprintf(stdout, "Modbus: Not all clients send data\n");
				modbusReconnect();
			}
			else
			{
				fprintf(stdout, "Modbus: Receive error\n");
				modbusReconnect();
			}
			

			printf("Id:%2d temp=%6d \t (0x%X)\n", clientsDataList.clients[0].clientId, 
											  clientsDataList.clients[0].data[0], 
											  clientsDataList.clients[0].data[0]);

			data.temp = clientsDataList.clients[0].data[0];
			// modbus_connect end

			time(&data.time);

			// mqtt_send("Test\n", "temp");
			send_data(data);
			send_mqtt_1m = false;
		}

		sleep(1);

	}

	// modbus_connect begin
	// modbusDeinit();
	// modbus_connect end
	
	// mosquitto_loop_forever(mosq, -1, 1);
	// mosquitto_destroy(mosq);
	// mosquitto_lib_cleanup();
	return 0;
}











/*
int main(int argc, char **argv)
{

	int                   clientNum;
	ModbusError           status;
	ModbusClientsList     clientsList;
	ModbusClientsDataList clientDataList;

	// Setup client
	clientsList.clientsCnt = 2;               // Two client

	// First client
	clientsList.clients[0].id = 1;            // Client id
	clientsList.clients[0].port = 502;        // Port
	clientsList.clients[0].offset = 0;        // Bytes offset
	clientsList.clients[0].numOfBytes = 1;    // Bytes num
	snprintf(clientsList.clients[0].ipAdress, IP_BUF_SIZE, "%s", "192.168.1.226"); // Ip

	// Second client
	clientsList.clients[1].id = 2;            // Client id
	clientsList.clients[1].port = 5002;       // Port
	clientsList.clients[1].offset = 0;        // Bytes offset
	clientsList.clients[1].numOfBytes = 1;    // Bytes num
	snprintf(clientsList.clients[1].ipAdress, IP_BUF_SIZE, "%s", "192.168.1.112"); // Ip


	fprintf(stdout, "Start\n");

	// Init
	status = modbusInit(&clientsList);

	if(status == MBE_OK)
	{
		fprintf(stdout, "Init OK\n");
	}
	else if(status == MBE_NOT_ALL)
	{
		fprintf(stdout, "Not all clients inited\n");
	}
	else
	{
		fprintf(stdout, "Init error\n");
		modbusDeinit();
		return 1;
	}

	// Receive data
	status = modbusReceiveData(&clientDataList);

	if(status == MBE_OK)
	{
		fprintf(stdout, "Receive OK\n");
	}
	else if(status == MBE_NOT_ALL)
	{
		fprintf(stdout, "Not all clients send data\n");
	}
	else
	{
		fprintf(stdout, "Receive error\n");
		modbusDeinit();
		return 1;
	}

	// Print data
	for(clientNum = 0; clientNum < clientsList.clientsCnt; clientNum++)
	{

		printf("Id:%2d temp=%6d \t (0x%X)\n", clientDataList.clients[clientNum].clientId, 
											  clientDataList.clients[clientNum].data[0], 
											  clientDataList.clients[clientNum].data[0]);
	}

	modbusDeinit();

    return 0;
}

*/