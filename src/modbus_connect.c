#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <modbus.h>
#include <stdbool.h>
#include "modbus_connect.h"

#define MODBUS_DEBUG 1

#define COMMAND_BUF_SIZE 16 // svv delete
#define RCV_BUF_SIZE 100    // svv delete

/*
#define LOG_MSG(msg) log_msg(msg, __FUNCTION__, __LINE__)
#define ERR_MSG(msg) err_msg(msg, __FUNCTION__, __LINE__)
 
void log_msg(const char *msg, const char *func, const int line)
{
	if(MODBUS_DEBUG)
    	printf("MSG: %s Fun: %s line: %d\n", msg, func, line);
}

void err_msg(const char *msg, const char *func, const int line)
{
	if(MODBUS_DEBUG)
    	printf(stderr, "ERROR: %s Fun: %s line: %d\n", msg, func, line);
    else
    	printf(stderr, "ERROR: %s\n", msg);
}
*/

ModbusClientsList _clientsList;

ModbusError modbusInit(ModbusClientsList *pClientsList)
{
	int  clientNum;
	bool atLeastOne = false;
	ModbusError mbStatus = MBE_OK;

	if(pClientsList->clientsCnt == 0)
	{
		fprintf(stderr, "ERROR: No clients");
		return MBE_CLIENT;
	}
	else if(pClientsList->clientsCnt > MAX_CLIENT_NUM)
	{
		fprintf(stderr, "ERROR: Too much clients, max  %d\n", MAX_CLIENT_NUM);
		return MBE_CLIENT;
	}

	printf("Find %d clients\n", pClientsList->clientsCnt);

	// Save clients data in _clientsList
	memcpy(&_clientsList, pClientsList, sizeof(ModbusClientsList));

	// Use _clientsList for connection
	for(clientNum = 0; clientNum < _clientsList.clientsCnt; clientNum++)
	{
		// Get context
		_clientsList.clientsList[clientNum].context = modbus_new_tcp(_clientsList.clientsList[clientNum].ipAdress, 
																	 _clientsList.clientsList[clientNum].port);	
		
		if (_clientsList.clientsList[clientNum].context == NULL) 
		{
			fprintf(stderr, "ERROR: Unable to allocate libmodbus context for ip: %s, port: %d\n", 
					_clientsList.clientsList[clientNum].ipAdress,
					_clientsList.clientsList[clientNum].port);

			return MBE_CONTEXT;
		}

		// Connect
		if (modbus_connect(_clientsList.clientsList[clientNum].context) == -1) 
		{
			fprintf(stderr, "ERROR: Connection failed: %s, for ip: %s, port: %d\n", modbus_strerror(errno),
					_clientsList.clientsList[clientNum].ipAdress,
					_clientsList.clientsList[clientNum].port);

			mbStatus = MBE_FAIL;
			continue;
		}

		modbus_set_slave(_clientsList.clientsList[clientNum].context, _clientsList.clientsList[clientNum].id);

		_clientsList.clientsList[clientNum].connected = true;
		atLeastOne = true;

		printf("Connected to ip: %s, port: %d\n", 
				_clientsList.clientsList[clientNum].ipAdress,
				_clientsList.clientsList[clientNum].port);
	}

	// Check status
	if(mbStatus != MBE_OK)
	{
		if(atLeastOne)
			return MBE_NOT_ALL;
		else
			return MBE_FAIL;
	}

	return MBE_OK;
}

ModbusError modbusReceiveData(ModbusClientsDataList *pDataList)
{
	int rc;
	int  clientNum;
	bool atLeastOne = false;
	ModbusError mbStatus = MBE_OK;

	memset(pDataList, '\0', sizeof(ModbusClientsDataList));

	for(clientNum = 0; clientNum < _clientsList.clientsCnt; clientNum++)
	{
		if(_clientsList.clientsList[clientNum].connected)
		{
			pDataList->clientsDataList[clientNum].clientId = _clientsList.clientsList[clientNum].id;

			rc = modbus_read_registers(_clientsList.clientsList[clientNum].context, 
									   _clientsList.clientsList[clientNum].offset, 
									   _clientsList.clientsList[clientNum].numOfBytes, 
									   pDataList->clientsDataList[clientNum].clientData);

			if (rc == -1) 
			{
				fprintf(stderr, "ERROR: Recive data: %s, from ip: %s, port: %d\n", modbus_strerror(errno), 
					    _clientsList.clientsList[clientNum].ipAdress, 
					    _clientsList.clientsList[clientNum].port);

				_clientsList.clientsList[clientNum].connected = false;
				mbStatus = MBE_FAIL;
				continue;	
			}

			atLeastOne = true;
		}
	}

	// Check status
	if(mbStatus != MBE_OK)
	{
		if(atLeastOne)
			return MBE_NOT_ALL;
		else
			return MBE_FAIL;
	}

	return MBE_OK;
}

ModbusError modbusReconnect()
{
	int  clientNum;
	bool atLeastOne = false;
	ModbusError mbStatus = MBE_OK;

	for(clientNum = 0; clientNum < _clientsList.clientsCnt; clientNum++)
	{
		if(_clientsList.clientsList[clientNum].connected == false)
		{
			// Client not connected
			if(_clientsList.clientsList[clientNum].context != NULL)	
			{
				modbus_close(_clientsList.clientsList[clientNum].context);	
				modbus_free(_clientsList.clientsList[clientNum].context);	

				// Get context
				_clientsList.clientsList[clientNum].context = modbus_new_tcp(_clientsList.clientsList[clientNum].ipAdress, 
																			 _clientsList.clientsList[clientNum].port);	
				
				if (_clientsList.clientsList[clientNum].context == NULL) 
				{
					fprintf(stderr, "ERROR: Unable to allocate libmodbus context for ip: %s, port: %d\n", 
							_clientsList.clientsList[clientNum].ipAdress,
							_clientsList.clientsList[clientNum].port);

					return MBE_CONTEXT;
				}

				// Connect
				if (modbus_connect(_clientsList.clientsList[clientNum].context) == -1) 
				{
					fprintf(stderr, "ERROR: Connection failed: %s, for ip: %s, port: %d\n", modbus_strerror(errno),
							_clientsList.clientsList[clientNum].ipAdress,
							_clientsList.clientsList[clientNum].port);

					mbStatus = MBE_FAIL;
					continue;
				}

				modbus_set_slave(_clientsList.clientsList[clientNum].context, _clientsList.clientsList[clientNum].id);	

			}
			else
			{
				fprintf(stderr, "ERROR: No context, need reinit, client ip: %s, port: %d\n", 
					    _clientsList.clientsList[clientNum].ipAdress, 
					    _clientsList.clientsList[clientNum].port);

				return MBE_REINIT;
			}

		}
	}

	// Check status
	if(mbStatus != MBE_OK)
	{
		if(atLeastOne)
			return MBE_NOT_ALL;
		else
			return MBE_FAIL;
	}

	return MBE_OK;	
}

void modbusDeinit()
{
	int  clientNum;

	for(clientNum = 0; clientNum < _clientsList.clientsCnt; clientNum++)
	{
		if(_clientsList.clientsList[clientNum].context != NULL)	
		{
			modbus_close(_clientsList.clientsList[clientNum].context);	
			modbus_free(_clientsList.clientsList[clientNum].context);
		}
	}
}