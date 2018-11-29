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
#include "LockDriver.h"
#include "RgbLedControl.h"
#include "BoxCodeParser.h"
#include "CommHandler.h"

class CommHandler;  // forward declaration to avoid error caused by recursive inclusion (Box.h includes CommHandler.h and vice versa)

class Box {
public:
	Box(gpio_num_t lockPin, gpio_num_t ledRedPin, gpio_num_t ledGreenPin, gpio_num_t ledBluePin, gpio_num_t buttonPin);
	void            init();
	void			setOwner(User* pOwner);
	void            setCommHandler(CommHandler* pCommHandler);
	//void            setLockDriver(LockDriver* pLockDriver);
	//void            setRgbLedControl(RgbLedControl* pRgbLedControl);
	//void            setBoxCodeParser(BoxCodeParser* pBoxCodeParser);
	User*			getOwner();
	LockDriver*     getLockDriver();
	RgbLedControl*  getRgbLedControl();
	BoxCodeParser*  getBoxCodeParser();
private:
	User*	    	m_pOwner;
	LockDriver*     m_pLockDriver;
	RgbLedControl*  m_pRgbLedControl;
	BoxCodeParser*  m_pBoxCodeParser;
	CommHandler*    m_pCommHandler;
};

#endif /* COMPONENTS_BOX_H_ */
