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
    m_pSocket = nullptr;
    //m_pSysControl = nullptr;
    m_pBox = nullptr;
    m_pSocketBoard = nullptr;
    m_sessionToken = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    m_sendDataQueue = nullptr;
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
	        m_sessionToken = data.getObject(0).getString("token").c_str();
	        ESP_LOGD(LOG_TAG, "MSG_ID_INIT received, %d data elements, trust number = %s", numberOfDataElements, m_sessionToken.c_str());
	        break;
	    }
		case MSG_ID_ACK: {

			break;
		}
		case MSG_ID_NACK: {

			break;
		}
		case MSG_ID_ACTIVATESOCKET: {
		    if(m_pSocketBoard != nullptr && m_pBox == nullptr){
                JsonArray data = obj.getArray("data");
                uint32_t numberOfDataElements = data.size();
                JsonObject maxTimeObject = data.getObject(0);
                m_pSocketBoard->setMaxTime(maxTimeObject.getInt("maxTime"));
                ESP_LOGD(LOG_TAG, "MSG_ID_ACTIVATESOCKET received, %d data elements, maxTime = %d", numberOfDataElements, m_pSocketBoard->getMaxTime());
                // HN-CHECK: Todo: switch on socket for maxTime
                m_pSocketBoard->switchOn();
		    }
			break;
		}
		case MSG_ID_DEACTIVATESOCKET: {
            if(m_pSocketBoard != nullptr && m_pBox == nullptr){
                ESP_LOGD(LOG_TAG, "MSG_ID_DEACTIVATESOCKET received");
                // HN-CHECK: Todo: switch off socket, reset maxTime timer
                // and send MSG_ID_DEACTIVATESOCKET_RESP
                uint32_t duration_sec = m_pSocketBoard->switchOff(false);

                CommHandlerSendData_t sendData;
                sendData.msgID = MSG_ID_SETSOCKETEVENT;
                sendData.value1 = duration_sec;
                sendData.value2 = 0;
                sendData.flag1 = false;
                addSendMessage(sendData);
                //sendSocketBoardEvent();
            }
			break;
		}
		case MSG_ID_REQUESTOPEN_RESP: {
            if(m_pBox != nullptr && m_pSocketBoard == nullptr){

            }
			break;
		}
		case MSG_ID_GETBOXDATA_RESP: {
            if(m_pBox != nullptr && m_pSocketBoard == nullptr){

            }
			break;
		}
		case MSG_ID_STARTBOXCODEPARSING: {
            if(m_pBox != nullptr && m_pSocketBoard == nullptr){

            }
			break;
		}
		case MSG_ID_STOPBOXCODEPARSING: {
            if(m_pBox != nullptr && m_pSocketBoard == nullptr){

            }
			break;
		}
		case MSG_ID_LOCKBOX: {
            if(m_pBox != nullptr && m_pSocketBoard == nullptr){

            }
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
bool CommHandler::sendData(CommHandlerSendData_t sendData){

    JsonObject obj = JSON::createObject();
    obj.setString("token", m_sessionToken);
    obj.setInt("uid", esp_random());

    switch(sendData.msgID){
        case MSG_ID_ACK: {
            obj.setInt("type", MSG_ID_ACK);
            JsonArray data = JSON::createArray();
            JsonObject refuid = JSON::createObject();
            refuid.setInt("refuid", sendData.value1);
            data.addObject(refuid);
            obj.setArray("data", data);
            ESP_LOGD(LOG_TAG, "Send MSG_ID_ACK: %s", obj.toStringUnformatted().c_str());
            break;
        }
        case MSG_ID_NACK: {
            obj.setInt("type", MSG_ID_NACK);
            JsonArray data = JSON::createArray();
            JsonObject errornumber = JSON::createObject();
            errornumber.setInt("errno", sendData.value2);
            JsonObject message = JSON::createObject();
            message.setString("message", "ESP32 NACK");
            JsonObject refuid = JSON::createObject();
            refuid.setInt("refuid", sendData.value1);
            data.addObject(errornumber);
            data.addObject(message);
            data.addObject(refuid);
            obj.setArray("data", data);
            ESP_LOGD(LOG_TAG, "Send MSG_ID_NACK: %s", obj.toStringUnformatted().c_str());
            break;
        }
        case MSG_ID_REQUESTOPEN: {
            obj.setInt("type", MSG_ID_REQUESTOPEN);
            ESP_LOGD(LOG_TAG, "Send MSG_ID_REQUESTOPEN: %s", obj.toStringUnformatted().c_str());

            break;
        }
        case MSG_ID_GETBOXDATA: {
            obj.setInt("type", MSG_ID_GETBOXDATA);
            ESP_LOGD(LOG_TAG, "Send MSG_ID_GETBOXDATA: %s", obj.toStringUnformatted().c_str());
            break;
        }
        case MSG_ID_SETBOXEVENT: {
            obj.setInt("type", MSG_ID_SETBOXEVENT);
            ESP_LOGD(LOG_TAG, "Send MSG_ID_SETBOXEVENT: %s", obj.toStringUnformatted().c_str());
            break;
        }
        case MSG_ID_SETSOCKETEVENT: {
            obj.setInt("type", MSG_ID_SETSOCKETEVENT);
            JsonArray data = JSON::createArray();
            JsonObject duration = JSON::createObject();
            duration.setInt("duration", sendData.value1);
            JsonObject deltaTime = JSON::createObject();
            deltaTime.setInt("deltaTime", sendData.value2);
            JsonObject isTimeout = JSON::createObject();
            isTimeout.setBoolean("isTimeout", sendData.flag1);
            data.addObject(duration);
            data.addObject(deltaTime);
            data.addObject(isTimeout);
            obj.setArray("data", data);
            ESP_LOGD(LOG_TAG, "Send MSG_ID_SETSOCKETEVENT: %s", obj.toStringUnformatted().c_str());
            break;
        }
        default: {
            return false;
        }
    }

    std::string str = obj.toStringUnformatted();
    sendEncodedRawData(str.c_str(), WS_OPCODE_TEXT);

    return true;
}


/**
 * @brief xx
 */
bool CommHandler::addSendMessage(CommHandlerSendData_t sendData){
    if(xQueueSend(m_sendDataQueue,(void *)&sendData,(TickType_t )0)){       // HN-CHECK todo: check if sendData is valid!?
        return true;
    }
    return false;
}


/**
 * @brief xx
 */
bool CommHandler::sendEncodedRawData(char const *str, uint8_t opcode) {
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

	// print full receive buffer - DEBUG
	m_pReceiveBuffer[511] = '\0';
	ESP_LOGD(LOG_TAG, "received string = %s", m_pReceiveBuffer);

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
    sendEncodedRawData("", WS_OPCODE_CLOSE);
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


/**
 * @brief CommHandlerSendTask
 */
class CommHandlerSendTask: public Task {
public:
    CommHandlerSendTask(std::string name): Task(name, 16 * 1024) {
        m_pCommHandler = nullptr;
    };

private:
    CommHandler* m_pCommHandler; // Reference to the CommHandler

    /**
     * @brief xxx
     */
    void run(void* data) {
        m_pCommHandler = (CommHandler*) data;   // The passed in data is an instance of an CommHandler.
        CommHandlerSendData_t sendData;
        sendData.msgID = 999;
        sendData.value1 = 0;
        sendData.value2 = 0;
        sendData.value3 = 0;
        sendData.flag1 = false;
        sendData.flag2 = false;

        while (true) {   // Loop forever.
            ESP_LOGD("CommHandlerSendTask", "Waiting for data to send");
            if(xQueueReceive(m_pCommHandler->m_sendDataQueue,&sendData,pdMS_TO_TICKS(1000))){
                m_pCommHandler->sendData(sendData);  // send out data if there is something added to the queue
            }
        } // while
    } // run
}; // CommHandlerSendTask


void CommHandler::start(){
    m_sendDataQueue = xQueueCreate(10,sizeof(CommHandlerSendData_t));
    if(!m_sendDataQueue){
        ESP_LOGD(LOG_TAG, "Failed to create sendDataQueue!");
    }

    CommHandlerSendTask* pCommHandlerSendTask = new CommHandlerSendTask("CommHandlerSendTask");
    pCommHandlerSendTask->start(this);

    CommHandlerReceiveTask* pCommHandlerReceiveTask = new CommHandlerReceiveTask("CommHandlerReceiveTask");
    pCommHandlerReceiveTask->start(this);
}

