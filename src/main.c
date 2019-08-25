#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "modbus_connect.h"

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