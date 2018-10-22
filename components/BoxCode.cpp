/*
 * BoxCode.cpp
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */


#include "BoxCode.h"

/**
 * @brief Create a BoxCode instance.
 */
BoxCode::BoxCode(){
	m_boxCode	= 0;
	m_isCodeSet = false;
}


/**
 * @brief Parse box code (evaluate button presses and set m_boxCode), return true on success
 */
bool BoxCode::parseCode(){
	// do something
	// if success:
	//   m_isCodeSet = true;
	return true;
}


/**
 * @brief Compare if *pCode and m_boxCode are equal (overload == operator)
 */
bool BoxCode::compareCode(BoxCode* pCode){
	if(m_isCodeSet && (m_boxCode == *pCode)){
		return true;
	}
	return false;
}


/**
 * @brief Return true if a code was set by parseCode()
 */
bool BoxCode::isCodeSet(){
	return m_isCodeSet;
}

