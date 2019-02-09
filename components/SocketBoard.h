/********************************************************************************************
 * Project    : Rewardi
 * Created on : 22.10.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : SocketBoard.h
 * Purpose    : Represents a Rewardi SocketBoard
 ********************************************************************************************/

#ifndef COMPONENTS_SOCKETBOARD_H_
#define COMPONENTS_SOCKETBOARD_H_

#include <stdint.h>
#include <FreeRTOS.h>
#include <freertos/timers.h>
#include "RelaisDriver.h"
#include "CommHandler.h"

class CommHandler;  // forward declaration to avoid error caused by recursive inclusion (SocketBoard.h includes CommHandler.h and vice versa)

class SocketBoard {
public:
	SocketBoard(gpio_num_t relaisPin);
	~SocketBoard();
	void            init();
	void            setSocketID(uint32_t socketID);
	void			setMaxTime(uint32_t maxTime);
    void            setCommHandler(CommHandler* pCommHandler);
	uint32_t		getSocketID();
	uint32_t		getMaxTime();
	RelaisDriver*   getRelaisDriver();
	void            switchOn();
	uint32_t        switchOff(bool isTimeout);
	static void timeout(TimerHandle_t xTimer);
private:
	uint32_t		m_socketID;
	uint32_t		m_maxTime;
	RelaisDriver*   m_pRelaisDriver;
	TimerHandle_t   m_hTimeout;
	CommHandler*    m_pCommHandler;
};

#endif /* COMPONENTS_SOCKETBOARD_H_ */
