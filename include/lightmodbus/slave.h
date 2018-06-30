/*
	liblightmodbus - a lightweight, multiplatform Modbus library
	Copyright (C) 2017 Jacek Wieczorek <mrjjot@gmail.com>

	This file is part of liblightmodbus.

	Liblightmodbus is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Liblightmodbus is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LIGHTMODBUS_SLAVE_BASE_H
#define LIGHTMODBUS_SLAVE_BASE_H

#include <inttypes.h>
#include "lightmodbus.h"
#include "parser.h"
#include "libconf.h"

//Struct associating user defined parser function with function ID
#ifdef LIGHTMODBUS_USER_FUNCTIONS
struct modbusSlave;
typedef struct modbusUserFunction
{
	uint8_t function; //Function code
	ModbusError ( *handler )( struct modbusSlave *status, union modbusParser *parser ); //Pointer to user defined function
} ModbusUserFunction;
#endif

typedef struct modbusSlave
{
	uint8_t address; //Slave address

	uint16_t *registers; //Slave holding registers
	uint16_t registerCount; //Slave register count
	uint16_t *inputRegisters; //Slave input registers
	uint16_t inputRegisterCount; //Slave input count
	uint8_t *coils; //Slave coils
	uint16_t coilCount; //Slave coil count
	uint8_t *discreteInputs; //Slave discrete input
	uint16_t discreteInputCount; //Slave discrete input count

	uint8_t *registerMask; //Masks for register write protection (bit of value 1 - write protection)
	uint16_t registerMaskLength; //Masks length (each byte covers 8 registers)
	uint8_t *coilMask; //Masks for coil write protection (bit of value 1 - write protection)
	uint16_t coilMaskLength; //Masks length (each byte covers 8 coils)

	//Exception code of the last exception generated by modbusBuileException
	ModbusExceptionCode lastException; 

	//Array of user defined functions - these can override default Modbus functions
	#ifdef LIGHTMODBUS_USER_FUNCTIONS
		ModbusUserFunction *userFunctions;
		uint16_t userFunctionCount;
	#endif

	struct //Slave response formatting status
	{
		#ifdef LIGHTMODBUS_STATIC_MEM_SLAVE_RESPONSE
			uint8_t frame[LIGHTMODBUS_STATIC_MEM_SLAVE_RESPONSE];
		#else
			uint8_t *frame;
		#endif
		uint8_t length;
	} response;

	struct //Request from master should be put here
	{
		#ifdef LIGHTMODBUS_STATIC_MEM_SLAVE_REQUEST
			uint8_t frame[LIGHTMODBUS_STATIC_MEM_SLAVE_REQUEST];
		#else
			const uint8_t *frame;
		#endif
		uint8_t length;
	} request;

} ModbusSlave; //Type containing slave device configuration data

//Function prototypes
extern ModbusError modbusBuildException( ModbusSlave *status, uint8_t function, ModbusExceptionCode exceptionCode ); //Build an exception
extern ModbusError modbusParseRequest( ModbusSlave *status ); //Parse and interpret given modbus frame on slave-side
extern ModbusError modbusSlaveInit( ModbusSlave *status ); //Very basic init of slave side
extern ModbusError modbusSlaveEnd( ModbusSlave *status ); //Free memory used by slave

#endif
