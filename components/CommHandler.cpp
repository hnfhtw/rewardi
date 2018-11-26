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
#include "Task.h"

static const char* LOG_TAG = "CommHandler";

/**
 * @brief Create a CommHandler instance.
 */
CommHandler::CommHandler(){
    m_pSocket = NULL;
    //m_pSysControl = NULL;
    m_pBox = NULL;
    m_pSocketBoard = NULL;
    m_sessionToken = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
}

/*void CommHandler::setSysControl(SysControl* pSysControl){
    m_pSysControl = pSysControl;
}

SysControl* CommHandler::getSysControl(){
    return m_pSysControl;
}*/

void CommHandler::setBox(Box* pBox){
	m_pBox = pBox;
}

Box* CommHandler::getBox(){
	return m_pBox;
}

void CommHandler::setSocketBoard(SocketBoard* pSocketBoard){
    m_pSocketBoard = pSocketBoard;
}

SocketBoard* CommHandler::getSocketBoard(){
    return m_pSocketBoard;
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
	uint32_t msgID = obj.getInt("type");            // type = msgID
	ESP_LOGD(LOG_TAG, "Parsed MSG_ID = %d", msgID);

	switch(msgID){
	    case MSG_ID_INIT: {
	        JsonArray data = obj.getArray("data");
	        uint32_t numberOfDataElements = data.size();
	        ESP_LOGD(LOG_TAG, "Number of array elements = %d", numberOfDataElements);
	        m_sessionToken = data.getObject(0).getString("value").c_str();
	        ESP_LOGD(LOG_TAG, "MSG_ID_INIT received, %d data elements, trust number = %s", numberOfDataElements, m_sessionToken.c_str());
	        break;
	    }
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
		    JsonArray data = obj.getArray("data");
		    uint32_t numberOfDataElements = data.size();
		    JsonObject maxTimeObject = data.getObject(0);
		    m_pSocketBoard->setMaxTime(maxTimeObject.getInt("value"));
		    ESP_LOGD(LOG_TAG, "MSG_ID_ACTIVATESOCKET received, %d data elements, maxTime = %d", numberOfDataElements, m_pSocketBoard->getMaxTime());
		    // HN-CHECK: Todo: switch on socket for maxTime
		    m_pSocketBoard->switchOn();
			break;
		}
		case MSG_ID_DEACTIVATESOCKET: {
            ESP_LOGD(LOG_TAG, "MSG_ID_DEACTIVATESOCKET received");
            // HN-CHECK: Todo: switch off socket, reset maxTime timer
            // and send MSG_ID_DEACTIVATESOCKET_RESP
            m_pSocketBoard->switchOff(false);
            sendSocketBoardEvent();
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
/*bool CommHandler::sendLogin(){
	JsonObject obj = JSON::createObject();
	obj.setInt("type", MSG_ID_LOGIN);          // type = msgID
	JsonObject objData = JSON::createObject();
	objData.setString("trustNumber", m_pSysControl->getTrustNumber());
	obj.setObject("payload", objData);
	sendEncodedData(obj.toString().c_str(), 1);
	return true;
}*/

/**
 * @brief xx
 */
bool CommHandler::sendSocketBoardEvent(){
    JsonObject obj = JSON::createObject();
    obj.setString("token", m_sessionToken);
    obj.setInt("uid", 0);
    obj.setInt("type", MSG_ID_DEACTIVATESOCKET);          // type = msgID
    JsonArray data = JSON::createArray();
        JsonObject onTime = JSON::createObject();
        onTime.setString("param", "onTime");
        onTime.setString("value", "2018-11-24T23:00:00");
        JsonObject deltaTime = JSON::createObject();
        deltaTime.setString("param", "deltaTime");
        deltaTime.setInt("value", 3600);
        JsonObject isTimeout = JSON::createObject();
        isTimeout.setString("param", "isTimeout");
        isTimeout.setBoolean("value", false);
    data.addObject(onTime);
    data.addObject(deltaTime);
    data.addObject(isTimeout);
    obj.setArray("data", data);
    ESP_LOGD(LOG_TAG, "Send socketBoardEvent: %s", obj.toStringUnformatted().c_str());
    std::string str = obj.toStringUnformatted();
    sendEncodedData(str.c_str(), 1);
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
    uint8_t frameBuffer[1024];
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
        if (bufferIndex == 512) {
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
	while(bytesRead == 0){
	    bytesRead = m_pSocket->receive(m_pReceiveBuffer, 512, false);
	}

    ESP_LOGD(LOG_TAG, "received byte 0 = %d", m_pReceiveBuffer[0]);
    ESP_LOGD(LOG_TAG, "received byte 1 = %d", m_pReceiveBuffer[1]);
    ESP_LOGD(LOG_TAG, "received byte 2 = %d", m_pReceiveBuffer[2]);
    ESP_LOGD(LOG_TAG, "received byte 3 = %d", m_pReceiveBuffer[3]);
    ESP_LOGD(LOG_TAG, "received byte 4 = %d", m_pReceiveBuffer[4]);
    ESP_LOGD(LOG_TAG, "received byte 5 = %d", m_pReceiveBuffer[5]);
    ESP_LOGD(LOG_TAG, "received byte 6 = %d", m_pReceiveBuffer[6]);
    ESP_LOGD(LOG_TAG, "received byte 7 = %d", m_pReceiveBuffer[7]);
    ESP_LOGD(LOG_TAG, "received byte 8 = %d", m_pReceiveBuffer[8]);

	for(uint32_t i = 0; i<512; ++i){
	    if(m_pReceiveBuffer[i] == 0){
	        m_pReceiveBuffer[i] = 'x';
	    }
	}
	m_pReceiveBuffer[511] = '\0'; // DEBUG
	ESP_LOGD(LOG_TAG, "received string = %s", m_pReceiveBuffer);
	//ESP_LOGD(LOG_TAG, "received string = %s", &m_pReceiveBuffer[2]);
	//ESP_LOGD(LOG_TAG, "received string = %s", &m_pReceiveBuffer[8]);
	//ESP_LOGD(LOG_TAG, "received string = %s", &m_pReceiveBuffer[40]);

	m_pReceiveBuffer[bytesRead] = '\0';	// mark end of valid data

	uint32_t websocketPayloadLenght = m_pReceiveBuffer[1] >> 1;
	if(websocketPayloadLenght < 126){
	    ESP_LOGD(LOG_TAG, "payload length = %d, received string = %s", websocketPayloadLenght, &m_pReceiveBuffer[4]);
	    parseMessage((const char*) &m_pReceiveBuffer[4]);
	}
	else if(websocketPayloadLenght == 126){
	    ESP_LOGD(LOG_TAG, "payload length = %d, received string = %s", websocketPayloadLenght, &m_pReceiveBuffer[6]);
	    parseMessage((const char*) &m_pReceiveBuffer[6]);
	}
	else if(websocketPayloadLenght == 127){
	    ESP_LOGD(LOG_TAG, "payload length = %d, received string = %s", websocketPayloadLenght, &m_pReceiveBuffer[12]);
	    parseMessage((const char*) &m_pReceiveBuffer[12]);
	}
	else{

	}
    ESP_LOGD(LOG_TAG, "%d bytes received", bytesRead);
	return true;
}

/**
 * @brief xx
 */
bool CommHandler::closeWebsocket(){
    sendEncodedData("", 8);
    return true;
}


/**
 * @brief CommHandlerReceiveTask
 */
class CommHandlerReceiveTask: public Task {
public:
    CommHandlerReceiveTask(std::string name): Task(name, 16 * 1024) {
        m_pCommHandler = nullptr;
    };

private:
    CommHandler* m_pCommHandler; // Reference to the CommHandler

    /**
     * @brief xxx
     */
    void run(void* data) {
        m_pCommHandler = (CommHandler*) data;   // The passed in data is an instance of an CommHandler.

        while (true) {   // Loop forever.
            ESP_LOGD("CommHandlerReceiveTask", "Waiting for new input data");
            m_pCommHandler->receiveData();
        } // while
    } // run
}; // CommHandlerReceiveTask



void CommHandler::start(){
    CommHandlerReceiveTask* pCommHandlerReceiveTask = new CommHandlerReceiveTask("CommHandlerReceiveTask");
    pCommHandlerReceiveTask->start(this);
}

