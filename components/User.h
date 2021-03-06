/********************************************************************************************
 * Project    : Rewardi
 * Created on : 22.10.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : User.h
 * Purpose    : Represents a Rewardi user -> used for the owner of the Rewardi Box
 ********************************************************************************************/

#ifndef COMPONENTS_USER_H_
#define COMPONENTS_USER_H_

#include <stdint.h>

class User {
public:
	User();
	User(uint32_t userID);
	User(uint32_t userID, uint8_t boxCode);
	void            setUserID(uint32_t userID);
	void			setBoxCode(uint8_t boxCode);
	uint32_t		getUserID();
	uint8_t	        getBoxCode();
private:
	uint32_t		m_userID;
	uint8_t			m_boxCode;
};


#endif /* COMPONENTS_USER_H_ */
