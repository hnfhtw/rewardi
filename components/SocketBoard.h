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

class SocketBoard {
public:
	SocketBoard();
	void            setSocketID(uint32_t socketID);
	void			setBackendURL(std::string backendURL);
	void			setMaxTime(double maxTime);
	uint32_t		getSocketID();
	double			getMaxTime();

private:
	uint32_t		m_socketID;
	std::string		m_backendURL;
	double			m_maxTime;
};

#endif /* COMPONENTS_SOCKETBOARD_H_ */
