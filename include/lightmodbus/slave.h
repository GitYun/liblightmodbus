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

#ifndef LIGHTMODBUS_SLAVE_H
#define LIGHTMODBUS_SLAVE_H

#include <inttypes.h>
#include "lightmodbus.h"
#include "parser.h"
#include "libconf.h"

#ifdef LIGHTMODBUS_SLAVE_BASE

//Struct associating user defined parser function with function ID
#ifdef LIGHTMODBUS_USER_FUNCTIONS
	struct modbusSlave;
	typedef struct modbusUserFunction
	{
		uint8_t function; //Function code
		ModbusError ( *handler )( struct modbusSlave *status, union modbusParser *parser ); //Pointer to user defined function
	} ModbusUserFunction;
#endif

//Register callback function
#if defined( LIGHTMODBUS_REGISTER_CALLBACK ) || defined( LIGHTMODBUS_COIL_CALLBACK )
	#ifndef LIGHTMODBUS_EXPERIMENTAL
		#error Register callback functions are an experimental feature that may cause problems. Please define LIGHTMODBUS_EXPERIMENTAL to dismiss this error message.
	#endif
	typedef enum modbusRegisterQuery
	{
		MODBUS_REGQ_R, //Requests callback function to return register value
		MODBUS_REGQ_W, //Requests callback function to write the register
		MODBUS_REGQ_R_CHECK, //Asks callback function if register can be read
		MODBUS_REGQ_W_CHECK //Asks callback function if register can be written
	} ModbusRegisterQuery;
	typedef uint16_t ( *ModbusRegisterCallbackFunction )( ModbusRegisterQuery query, ModbusDataType datatype, uint16_t index, uint16_t value );
#endif

typedef struct modbusSlave
{
	uint8_t address; //Slave address

	//Universal register/coil callback function
	#if defined( LIGHTMODBUS_COIL_CALLBACK ) || defined( LIGHTMODBUS_REGISTER_CALLBACK )
		ModbusRegisterCallbackFunction registerCallback;
	#endif

	//Slave holding registers array
	#ifndef LIGHTMODBUS_REGISTER_CALLBACK
		uint16_t *registers; //Register array
		uint16_t *inputRegisters; //Slave input registers
		uint8_t *registerMask; //Masks for register write protection (bit of value 1 - write protection)
		uint16_t registerMaskLength; //Masks length (each byte covers 8 registers)
	#endif
	uint16_t registerCount; //Slave register count
	uint16_t inputRegisterCount; //Slave input count

	//Slave coils array
	#ifndef LIGHTMODBUS_COIL_CALLBACK
		uint8_t *coils; //Slave coils
		uint8_t *discreteInputs; //Slave discrete input
		uint8_t *coilMask; //Masks for coil write protection (bit of value 1 - write protection)
		uint16_t coilMaskLength; //Masks length (each byte covers 8 coils)
	#endif
	uint16_t coilCount; //Slave coil count
	uint16_t discreteInputCount; //Slave discrete input count

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
#endif

//Function prototypes
#ifdef LIGHTMODBUS_SLAVE_BASE
extern ModbusError modbusBuildException( ModbusSlave *status, uint8_t function, ModbusExceptionCode exceptionCode ); //Build an exception
extern ModbusError modbusParseRequest( ModbusSlave *status ); //Parse and interpret given modbus frame on slave-side
extern ModbusError modbusSlaveInit( ModbusSlave *status ); //Very basic init of slave side
extern ModbusError modbusSlaveEnd( ModbusSlave *status ); //Free memory used by slave
#endif

#endif
