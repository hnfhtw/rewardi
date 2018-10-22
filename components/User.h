/*
 * User.h
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#ifndef COMPONENTS_USER_H_
#define COMPONENTS_USER_H_

#include <stdint.h>
#include "BoxCode.h"

class User {
public:
	User();
	void            setUserID(uint32_t userID);
	void			setRewardi(double rewardi);		// set rewardi status only if userID is set
	void			setBoxCode(BoxCode boxCode);
	uint32_t		getUserID();
	double			getRewardi();
	BoxCode			getBoxCode();
private:
	uint32_t		m_userID;
	double	    	m_rewardi;
	BoxCode			m_boxCode;
};


#endif /* COMPONENTS_USER_H_ */
