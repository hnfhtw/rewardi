/*
 * User.h
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#ifndef COMPONENTS_USER_H_
#define COMPONENTS_USER_H_

#include <stdint.h>

class User {
public:
	User();
	User(uint32_t userID);
	void            setUserID(uint32_t userID);
	void			setBoxCode(uint8_t boxCode);
	uint32_t		getUserID();
	uint8_t	        getBoxCode();
private:
	uint32_t		m_userID;
	uint8_t			m_boxCode;
};


#endif /* COMPONENTS_USER_H_ */
