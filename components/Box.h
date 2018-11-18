/*
 * Box.h
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#ifndef COMPONENTS_BOX_H_
#define COMPONENTS_BOX_H_

#include <stdint.h>
#include <string>
#include "User.h"

class Box {
public:
	Box();
	void            setBoxID(uint32_t boxID);
	void			setOwner(User* pOwner);
	void			setRewardiPerOpen(double rewardiPerOpen);
	uint32_t		getBoxID();
	User*			getOwner();
	double			getRewardiPerOpen();
private:
	uint32_t		m_boxID;
	User*	    	m_pOwner;
	double			m_rewardiPerOpen;
};

#endif /* COMPONENTS_BOX_H_ */
