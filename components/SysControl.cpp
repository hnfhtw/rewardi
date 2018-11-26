/*
 * SysControl.cpp
 *
 *  Created on: 16.11.2018
 *      Author: HN
 */

#include "SysControl.h"

/**
 * @brief Create a SysControl instance.
 */
SysControl::SysControl(){
	m_backendURL  		= "";
	m_trustNumber		= "1234ABCD";
}

/**
 * @brief xx
 */
void SysControl::setBackendURL(std::string backendURL){
	m_backendURL = backendURL;		// use string copy function??
}

/**
 * @brief xx
 */
void SysControl::setCommHandler(CommHandler* pCommHandler){
    m_pCommHandler = pCommHandler;
}

/**
 * @brief xx
 */
void SysControl::setSocketBoard(SocketBoard* pSocketBoard){
    m_pSocketBoard = pSocketBoard;
}

/**
 * @brief xx
 */
std::string SysControl::getBackendURL(){
	return m_backendURL;
}

/**
 * @brief xx
 */
std::string SysControl::getTrustNumber(){
	return m_trustNumber;
}

/**
 * @brief xx
 */
CommHandler* SysControl::getCommHandler(){
    return m_pCommHandler;
}

/**
 * @brief xx
 */
SocketBoard* SysControl::getSocketBoard(){
    return m_pSocketBoard;
}
