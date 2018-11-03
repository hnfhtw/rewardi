/*
 * BoxCode.h
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#ifndef COMPONENTS_BOXCODE_H_
#define COMPONENTS_BOXCODE_H_

#include <stdint.h>

class BoxCode {
public:
	BoxCode();
	bool            parseCode();                      // parse box code (evaluate button presses and set m_boxCode), return true on success
	bool			compareCode(BoxCode* pCode);	  // compare if *pCode and m_boxCode are equal (overload == operator)
	bool			isCodeSet();					  // return true if a code was set by parseCode()
private:
	uint8_t			m_boxCode;
	bool			m_isCodeSet;
};

#endif /* COMPONENTS_BOXCODE_H_ */
