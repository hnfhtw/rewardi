/*
 * SocketBoard.h
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#ifndef COMPONENTS_SOCKETBOARD_H_
#define COMPONENTS_SOCKETBOARD_H_

#include <stdint.h>

class SocketBoard {
public:
	SocketBoard();
	void            setSocketID(uint32_t socketID);
	void			setMaxTime(double maxTime);
	uint32_t		getSocketID();
	double			getMaxTime();

private:
	uint32_t		m_socketID;
	double			m_maxTime;
};

#endif /* COMPONENTS_SOCKETBOARD_H_ */
