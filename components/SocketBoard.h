/*
 * SocketBoard.h
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#ifndef COMPONENTS_SOCKETBOARD_H_
#define COMPONENTS_SOCKETBOARD_H_

#include <stdint.h>
#include <string>
#include "RelaisDriver.h"
//#include "SysControl.h"
#include <FreeRTOS.h>
#include <freertos/timers.h>


class SocketBoard {
public:
	SocketBoard();
	~SocketBoard();
	void            init();
	void            setSocketID(uint32_t socketID);
	void			setMaxTime(uint32_t maxTime);
	void            setRelaisDriver(RelaisDriver* pRelaisDriver);
	//void            setSysControl(SysControl* pSysControl);
	uint32_t		getSocketID();
	uint32_t		getMaxTime();
	RelaisDriver*   getRelaisDriver();
	//SysControl*     getSysControl();
	void            switchOn();
	uint32_t        switchOff(bool isTimeout);
	static void timeout(TimerHandle_t xTimer);
private:
	uint32_t		m_socketID;
	uint32_t		m_maxTime;
	RelaisDriver*   m_pRelaisDriver;
	TimerHandle_t   m_hTimeout;
	//SysControl*     m_pSysControl;
};

#endif /* COMPONENTS_SOCKETBOARD_H_ */
