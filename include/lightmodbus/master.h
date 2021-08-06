#ifndef LIGHTMODBUS_MASTER_H
#define LIGHTMODBUS_MASTER_H

#include <stdint.h>
#include <stddef.h>
#include "base.h"

typedef struct ModbusMaster ModbusMaster;

/**
	\brief A pointer to a response parsing function
*/
typedef ModbusErrorInfo (*ModbusMasterParsingFunction)(
	ModbusMaster *status,
	uint8_t address,
	uint8_t function,
	const uint8_t *requestPDU,
	uint8_t requestLength,
	const uint8_t *responsePDU,
	uint8_t responseLength);

/**
	\brief Associates Modbus function ID with a pointer to a response parsing function
*/
typedef struct ModbusMasterFunctionHandler
{
	uint8_t id;
	ModbusMasterParsingFunction ptr;
} ModbusMasterFunctionHandler;

/**
	\brief Arguments for the data callback
*/
typedef struct ModbusDataCallbackArgs
{
	ModbusDataType type; //!< Type of Modbus register
	uint16_t index;      //!< Index of the register
	uint16_t value;      //!< Value of the register
	uint8_t function;    //!< Function that reported this value
	uint8_t address;	 //!< Address of the slave
} ModbusDataCallbackArgs;

/**
	\brief A pointer to a callback used for handling data incoming to master
	\see master-data-callback
*/
typedef ModbusError (*ModbusDataCallback)(
	ModbusMaster *status,
	const ModbusDataCallbackArgs *args);

/**
	\brief A pointer to a callback called when a Modbus exception is generated (for master)
	\see master-exception-callback
*/
typedef ModbusError (*ModbusMasterExceptionCallback)(
	ModbusMaster *status,
	uint8_t address,
	uint8_t function,
	ModbusExceptionCode code);

/**
	\brief A pointer to master frame buffer allocator

	Please refer to \ref allocators for more information regarding custom allocator functions.
*/
typedef ModbusError (*ModbusMasterAllocator)(
	ModbusMaster *status,
	uint8_t **ptr,
	uint16_t size,
	ModbusBufferPurpose purpose);

/**
	\brief Master device status
*/
typedef struct ModbusMaster
{
	ModbusMasterAllocator allocator;                  //!< A pointer to an allocator function (required)
	ModbusDataCallback dataCallback;                  //!< A pointer to data callback (required)
	ModbusMasterExceptionCallback exceptionCallback;  //!< A pointer to an exception callback (optional)

	const ModbusMasterFunctionHandler *functions; //!< A non-owning pointer to array of function handlers
	uint8_t functionCount; //!< Size of \ref functions array

	void *context; //!< User's context pointer

	//! Stores master's request for slave
	ModbusFrameBuffer request;
} ModbusMaster;

LIGHTMODBUS_RET_ERROR modbusMasterInit(
	ModbusMaster *status,
	ModbusDataCallback dataCallback,
	ModbusMasterExceptionCallback exceptionCallback,
	ModbusMasterAllocator allocator,
	const ModbusMasterFunctionHandler *functions,
	uint8_t functionCount);

void modbusMasterDestroy(ModbusMaster *status);

LIGHTMODBUS_WARN_UNUSED ModbusError modbusMasterDefaultAllocator(ModbusMaster *status, uint8_t **ptr, uint16_t size, ModbusBufferPurpose purpose);
LIGHTMODBUS_WARN_UNUSED ModbusError modbusMasterAllocateRequest(ModbusMaster *status, uint16_t size);
void modbusMasterFreeRequest(ModbusMaster *status);

LIGHTMODBUS_RET_ERROR modbusBeginRequestPDU(ModbusMaster *status);
LIGHTMODBUS_RET_ERROR modbusEndRequestPDU(ModbusMaster *status);
LIGHTMODBUS_RET_ERROR modbusBeginRequestRTU(ModbusMaster *status);
LIGHTMODBUS_RET_ERROR modbusEndRequestRTU(ModbusMaster *status, uint8_t address);
LIGHTMODBUS_RET_ERROR modbusBeginRequestTCP(ModbusMaster *status);
LIGHTMODBUS_RET_ERROR modbusEndRequestTCP(ModbusMaster *status, uint16_t transaction, uint8_t unit);

LIGHTMODBUS_RET_ERROR modbusParseResponsePDU(
	ModbusMaster *status,
	uint8_t address,
	const uint8_t *request,
	uint8_t requestLength,
	const uint8_t *response,
	uint8_t responseLength);

LIGHTMODBUS_RET_ERROR modbusParseResponseRTU(
	ModbusMaster *status,
	const uint8_t *request,
	uint16_t requestLength,
	const uint8_t *response,
	uint16_t responseLength);

LIGHTMODBUS_RET_ERROR modbusParseResponseTCP(
	ModbusMaster *status,
	const uint8_t *request,
	uint16_t requestLength,
	const uint8_t *response,
	uint16_t responseLength);

/**
	\brief Returns a pointer to the request generated by the master
*/
LIGHTMODBUS_WARN_UNUSED static inline const uint8_t *modbusMasterGetRequest(const ModbusMaster *status)
{
	return status->request.data;
}

/**
	\brief Returns the length of the request generated by the master
*/
LIGHTMODBUS_WARN_UNUSED static inline uint16_t modbusMasterGetRequestLength(const ModbusMaster *status)
{
	return status->request.length;
}

/**
	\brief Allows user to set the custom context pointer
*/
static inline void modbusMasterSetUserPointer(ModbusMaster *status, void *ptr)
{
	status->context = ptr;
}

/**
	\brief Retreieves the custom context pointer
*/
static inline void *modbusMasterGetUserPointer(const ModbusMaster *status)
{
	return status->context;
}

extern ModbusMasterFunctionHandler modbusMasterDefaultFunctions[];
extern const uint8_t modbusMasterDefaultFunctionCount;

#endif