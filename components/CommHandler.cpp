/*
 * CommHandler.cpp
 *
 *  Created on: 03.11.2018
 *      Author: HN
 */

#include <esp_log.h>
#include "CommHandler.h"
#include "messageIDs.h"
#include <JSON.h>

static const char* LOG_TAG = "CommHandler";

/**
 * @brief Create a CommHandler instance.
 */
CommHandler::CommHandler(){
    m_pSocket = NULL;
    m_pSysControl = NULL;
    m_pBox = NULL;
}

void CommHandler::setSysControl(SysControl* pSysControl){
    m_pSysControl = pSysControl;
}

SysControl* CommHandler::getSysControl(){
    return m_pSysControl;
}

void CommHandler::setBox(Box* pBox){
	m_pBox = pBox;
}

Box* CommHandler::getBox(){
	return m_pBox;
}

void CommHandler::setSocket(Socket* pSocket){
	m_pSocket = pSocket;
}

Socket* CommHandler::getSocket(){
	return m_pSocket;
}

/**
 * @brief xx
 */
bool CommHandler::parseMessage(const char* message){
	JsonObject obj = JSON::parseObject(message);
	uint32_t msgID = obj.getInt("msgID");

	switch(msgID){
		case MSG_ID_LOGIN: {
			ESP_LOGD(LOG_TAG, "MSG_ID_LOGIN (echo) received");
			break;
		}
		case MSG_ID_LOGIN_RESP: {
			ESP_LOGD(LOG_TAG, "MSG_ID_LOGIN_RESP received");
			break;
		}
		case MSG_ID_ACK: {

			break;
		}
		case MSG_ID_NACK: {

			break;
		}
		case MSG_ID_GETUSERDATA_RESP: {

			break;
		}
		case MSG_ID_SETSOCKETID: {

			break;
		}
		case MSG_ID_SETBACKENDURL: {

			break;
		}
		case MSG_ID_GETSOCKETID_RESP: {

			break;
		}
		case MSG_ID_GETSOCKETDATA_RESP: {

			break;
		}
		case MSG_ID_ACTIVATESOCKET: {

			break;
		}
		case MSG_ID_DEACTIVATESOCKET: {

			break;
		}
		case MSG_ID_GETBOXID_RESP: {

			break;
		}
		case MSG_ID_GETBOXDATA_RESP: {

			break;
		}
		case MSG_ID_SETOWNER: {

			break;
		}
		case MSG_ID_STARTBOXCODEPARSING: {

			break;
		}
		case MSG_ID_STOPBOXCODEPARSING: {

			break;
		}
		case MSG_ID_LOCKBOX: {

			break;
		}
		default: {
			return false;
		}
	}

	return true;
}

/**
 * @brief xx
 */
bool CommHandler::sendLogin(){
	JsonObject obj = JSON::createObject();
	obj.setInt("msgID", MSG_ID_LOGIN);
	JsonObject objData = JSON::createObject();
	objData.setString("trustNumber", m_pSysControl->getTrustNumber());
	obj.setObject("payload", objData);
	sendEncodedData(obj.toString().c_str(), 1);
	return true;
}

/**
 * @brief xx
 */
bool CommHandler::sendEncodedData(char const *str, uint8_t opcode) {
	if(m_pSocket == NULL){
		return false;
	}
    uint8_t mask[4];
    uint8_t frameBuffer[128];
    size_t bufferIndex = 0;
    size_t bytesWritten = 0;
    int size = strlen(str);

    // Opcode; final fragment
    frameBuffer[bufferIndex] = (opcode | WS_FIN);
    bufferIndex++;

    // NOTE: no support for > 16-bit sized messages
    if (size > 125) {
        frameBuffer[bufferIndex] = (WS_SIZE16 | WS_MASK);
        frameBuffer[bufferIndex + 1] = ((uint8_t) (size >> 8));
        frameBuffer[bufferIndex + 2] = ((uint8_t) (size & 0xFF));
        bufferIndex += 3;
    } else {
        frameBuffer[bufferIndex] = ((uint8_t) size | WS_MASK);
        bufferIndex++;
    }

    mask[0] = rand();
    mask[1] = rand();
    mask[2] = rand();
    mask[3] = rand();

    frameBuffer[bufferIndex] = (mask[0]);
    frameBuffer[bufferIndex + 1] = (mask[1]);
    frameBuffer[bufferIndex + 2] = (mask[2]);
    frameBuffer[bufferIndex + 3] = (mask[3]);
    bufferIndex += 4;

    for (int i=0; i<size; ++i) {
        frameBuffer[bufferIndex] = (str[i] ^ mask[i % 4]);
        bufferIndex++;
        if (bufferIndex == 128) {
            bytesWritten += m_pSocket->send(frameBuffer, bufferIndex);
            bufferIndex = 0;
        }
    }
    if (bufferIndex > 0) {
        bytesWritten += m_pSocket->send(frameBuffer, bufferIndex);
    }
    return true;
}

/**
 * @brief xx
 */
bool CommHandler::receiveData(){
	uint32_t bytesRead = 0;
	bytesRead = m_pSocket->receive(m_pReceiveBuffer, 128, false);
	ESP_LOGD(LOG_TAG, "%d bytes received", bytesRead);
	m_pReceiveBuffer[bytesRead] = '\0';	// mark end of valid data
	parseMessage((const char*) &m_pReceiveBuffer[2]);
	return true;
}
