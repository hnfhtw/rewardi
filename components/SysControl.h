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
#include "User.h"

class SysControl {
public:
	SysControl();
	void			setBackendURL(std::string backendURL);
	std::string		getBackendURL();
	std::string     getTrustNumber();
private:
	std::string		m_backendURL;
	std::string		m_trustNumber;
};

#endif /* COMPONENTS_SYSCONTROL_H_ */
