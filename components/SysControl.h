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
class CommHandler;

class SysControl {
public:
    enum DeviceType{BOX=1,SOCKET=2};
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
    void            setDeviceType(DeviceType type);
    DeviceType      getDeviceType();
    void            setStayAwake(bool stayAwake);
    bool            getStayAwake();
private:
	std::string		m_backendURL;
	std::string		m_trustNumber;
    CommHandler*    m_pCommHandler;
    Socket*         m_pSocket;
    StandbyControl* m_pStandbyControl;
    DeviceType      m_deviceType;
    bool            m_stayAwake;
};

#endif /* COMPONENTS_SYSCONTROL_H_ */
