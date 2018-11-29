/*
 * SysControl.cpp
 *
 *  Created on: 16.11.2018
 *      Author: HN
 */

#include <esp_log.h>
#include "SysControl.h"

static const char* LOG_TAG = "SysControl";

/**
 * @brief Create a SysControl instance.
 */
SysControl::SysControl(){
	m_backendURL  		= "";
	m_trustNumber		= "1234ABCD";
	m_pSocket           = new Socket();
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
void SysControl::setTrustNumber(std::string trustNumber){
    m_trustNumber = trustNumber;
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

/**
 * @brief xx
 */
Socket* SysControl::getSocket(){
    return m_pSocket;
}

/**
 * @brief xx
 */
bool SysControl::connectToServer(){
    std::string websocketHandShakeRequest = "GET /ws/sockets/";
    websocketHandShakeRequest += getTrustNumber();
    websocketHandShakeRequest += " HTTP/1.1\r\nHost: ";
    websocketHandShakeRequest += getBackendURL();
    websocketHandShakeRequest += ":443\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Version: 13\r\n\r\n";

    m_pSocket->createSocket(false);  // create TCP socket for backend server connection
    m_pSocket->setSSL(true);         // initialize SSL (mbedTLS stack)
    m_pSocket->connect((char*) getBackendURL().c_str(), 443);     // establish TCP connection to backend server
    m_pSocket->sslHandshake();       // perform SSL handshake to establish SSL connection
        ESP_LOGD(LOG_TAG, " Websocket connected - SSL handshake done");     // HN-CHECK DEBUG
        ESP_LOGD(LOG_TAG, " Websocket handshake = %s", websocketHandShakeRequest.c_str());  // HN-CHECK DEBUG
    m_pSocket->send(websocketHandShakeRequest);  // send websocket handshake request -> this requests a protocol change from HTTP to Websocket
    vTaskDelay(500);
    uint8_t data[1024];
    m_pSocket->receive(data, 1024, false);       // receive websocket handshake response
    data[1023] = '\0';
        ESP_LOGD(LOG_TAG, " Websocket answer = %s", data);  // HN-CHECK DEBUG
    return m_pSocketBoard;
}

