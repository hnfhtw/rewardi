/********************************************************************************************
 * Project    : Rewardi
 * Created on : 22.10.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : Box.h
 * Purpose    : Represents a Rewardi Box
 ********************************************************************************************/

#ifndef COMPONENTS_BOX_H_
#define COMPONENTS_BOX_H_

#include <stdint.h>
#include <FreeRTOS.h>
#include <freertos/timers.h>
#include "User.h"
#include "LockDriver.h"
#include "RgbLedControl.h"
#include "BoxCodeParser.h"
#include "CommHandler.h"
#include "SysControl.h"

class CommHandler;  // forward declaration to avoid error caused by recursive inclusion (Box.h includes CommHandler.h and vice versa)
class BoxCodeParser;
class SysControl;
class LockDriver;


#define BOX_MAX_NR_OF_USERS 5

class Box {
public:
	Box(gpio_num_t lockPin, gpio_num_t ledRedPin, gpio_num_t ledGreenPin, gpio_num_t ledBluePin, gpio_num_t buttonPin);
	~Box();
	void            init();
    void            start();
	void			setOwner(uint32_t ownerID);
	void            setCommHandler(CommHandler* pCommHandler);
    void            setIsLocked(bool isLocked);
    void            setIsPendingOpenRequest(bool isPendingOpenRequest);
	User*			getOwner();
	LockDriver*     getLockDriver();
	RgbLedControl*  getRgbLedControl();
	BoxCodeParser*  getBoxCodeParser();
    bool            getIsLocked();
    bool            getIsPendingOpenRequest();
    void            requestBoxOpen(uint8_t boxCode);    // called by BoxCodeParser if a valid 5 digit box code was entered
    void            open();
    void            updateBoxData();
    void            setBoxCode(uint8_t boxCode);
    void            stayAwake();
    static void timeout(TimerHandle_t xTimer);
    SysControl*     m_pSysControl;
private:
	User*	    	m_pOwner;
    bool            m_isLocked;
    bool            m_isPendingOpenRequest;
	LockDriver*     m_pLockDriver;
	RgbLedControl*  m_pRgbLedControl;
	BoxCodeParser*  m_pBoxCodeParser;
	CommHandler*    m_pCommHandler;
	User*           m_userList[BOX_MAX_NR_OF_USERS];
	uint32_t        m_userListSize;
    bool            readUserListData();
    bool            writeUserListData();
    TimerHandle_t   m_hTimeout;
};

#endif /* COMPONENTS_BOX_H_ */
