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

class SocketBoard;

class SysControl {
public:
	SysControl();
	void			setBackendURL(std::string backendURL);
	void            setCommHandler(CommHandler* pCommHandler);
	void            setSocketBoard(SocketBoard* pSocketBoard);
	std::string		getBackendURL();
	std::string     getTrustNumber();
    CommHandler*    getCommHandler();
    SocketBoard*    getSocketBoard();
private:
	std::string		m_backendURL;
	std::string		m_trustNumber;
    CommHandler*    m_pCommHandler;
    SocketBoard*    m_pSocketBoard;
};

#endif /* COMPONENTS_SYSCONTROL_H_ */
