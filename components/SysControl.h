/*
 * SysControl.h
 *
 *  Created on: 16.11.2018
 *      Author: HN
 */

#ifndef COMPONENTS_SYSCONTROL_H_
#define COMPONENTS_SYSCONTROL_H_

#include <stdint.h>
#include <string>
#include "CommHandler.h"
#include "StandbyControl.h"

class SocketBoard;

class SysControl {
public:
	SysControl();
	void            init();
	void			setBackendURL(std::string backendURL);
	void            setTrustNumber(std::string trustNumber);
	void            setCommHandler(CommHandler* pCommHandler);
	std::string		getBackendURL();
	std::string     getTrustNumber();
    CommHandler*    getCommHandler();
    SocketBoard*    getSocketBoard();
    Socket*         getSocket();
    bool            connectToServer();
    void            enterDeepSleepMode();
    bool            readDeviceData(char* trustNumber, char* backendURL);
private:
	std::string		m_backendURL;
	std::string		m_trustNumber;
    CommHandler*    m_pCommHandler;
    Socket*         m_pSocket;
    StandbyControl* m_pStandbyControl;
};

#endif /* COMPONENTS_SYSCONTROL_H_ */
