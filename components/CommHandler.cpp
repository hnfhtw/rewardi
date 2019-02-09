/********************************************************************************************
 * Project    : Rewardi
 * Created on : 03.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : CommHandler.cpp
 * Purpose    : Responsible for Websocket communication with Rewardi Server
 ********************************************************************************************/

#include <esp_log.h>
#include "CommHandler.h"
#include "messageIDs.h"
#include <JSON.h>
#include "Task.h"

static const char* LOG_TAG = "CommHandler";

/**
 * @brief CommHandler constructor
 */
CommHandler::CommHandler(){
    m_pSocket = nullptr;
    m_pBox = nullptr;
    m_pSocketBoard = nullptr;
    m_sessionToken = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    m_sendDataQueue = nullptr;
    m_isConnected = false;
    m_pCommHandlerSendTask = nullptr;
    m_pCommHandlerReceiveTask = nullptr;
    m_pCommHandlerHeartbeatTask = nullptr;
}

/**
 * @brief Set pointer to Box object
 */
void CommHandler::setBox(Box* pBox){
	m_pBox = pBox;
}

/**
 * @brief Get pointer to Box object
 */
Box* CommHandler::getBox(){
	return m_pBox;
}

/**
 * @brief Set pointer to SocketBoard object
 */
void CommHandler::setSocketBoard(SocketBoard* pSocketBoard){
    m_pSocketBoard = pSocketBoard;
}

/**
 * @brief Get pointer to SocketBoard object
 */
SocketBoard* CommHandler::getSocketBoard(){
    return m_pSocketBoard;
}

/**
 * @brief Set pointer to TCP socket
 */
void CommHandler::setSocket(Socket* pSocket){
	m_pSocket = pSocket;
}

/**
 * @brief Get pointer to TCP socket
 */
Socket* CommHandler::getSocket(){
	return m_pSocket;
}

/**
 * @brief Get flag that indicates if Websocket communcation to server is established
 */
bool CommHandler::getIsConnected(){
    return m_isConnected;
}

/**
 * @brief Parse a message received from the Rewardi server
 */
bool CommHandler::parseMessage(const char* message){
	JsonObject obj = JSON::parseObject(message);
	uint32_t msgID = obj.getInt("type");            // type = msgID
	ESP_LOGD(LOG_TAG, "Parsed MSG_ID = %d", msgID);

	switch(msgID){
	    case MSG_ID_INIT: {
	        JsonObject data = obj.getObject("data");
	        m_sessionToken = data.getString("token").c_str();
	        ESP_LOGD(LOG_TAG, "MSG_ID_INIT received, session token = %s", m_sessionToken.c_str());
	        m_isConnected = true;
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
		        JsonObject data = obj.getObject("data");
		        uint32_t maxTime_sec = data.getInt("maxTime");
                m_pSocketBoard->setMaxTime(maxTime_sec);
                ESP_LOGD(LOG_TAG, "MSG_ID_ACTIVATESOCKET received, maxTime = %d", maxTime_sec);
                m_pSocketBoard->switchOn();     // switch on socket for maxTime
                CommHandlerSendData_t sendData;
                sendData.msgID = MSG_ID_ACK;
                sendData.value1 = obj.getInt("uid");
                addSendMessage(sendData);   // send MSG_ID_ACK to server
		    }
			break;
		}
		case MSG_ID_DEACTIVATESOCKET: {
            if(m_pSocketBoard != nullptr && m_pBox == nullptr){
                ESP_LOGD(LOG_TAG, "MSG_ID_DEACTIVATESOCKET received");
                uint32_t duration_sec = m_pSocketBoard->switchOff(false);   // switch off socket, stop maxTime timer

                CommHandlerSendData_t sendData;
                sendData.msgID = MSG_ID_ACK;
                sendData.value1 = obj.getInt("uid");
                addSendMessage(sendData);   // send MSG_ID_ACK to server

                sendData.msgID = MSG_ID_SETSOCKETEVENT;
                sendData.value1 = duration_sec;
                sendData.value2 = 0;
                sendData.flag1 = false;
                addSendMessage(sendData);   // send MSG_ID_SETSOCKETEVENT to server
            }
			break;
		}
		case MSG_ID_REQUESTOPEN_RESP: {
            if(m_pBox != nullptr && m_pSocketBoard == nullptr){
                JsonObject data = obj.getObject("data");
                bool isAllowed = data.getBoolean("isAllowed");
                ESP_LOGD(LOG_TAG, "MSG_ID_REQUESTOPEN_RESP received, isAllowed = %d", isAllowed);
                if(isAllowed){    // switch on allowed
                    m_pBox->open();
                    CommHandlerSendData_t sendData;
                    sendData.msgID = MSG_ID_SETBOXEVENT;
                    addSendMessage(sendData);   // send MSG_ID_SETBOXEVENT to server, HN-CHECK TODO: Send it only when box was really opened!
                }
                else{   // switch on not allowed
                    m_pBox->getRgbLedControl()->setColor(RgbLedControl::Color::RED);
                    m_pBox->getRgbLedControl()->setPeriod(RgbLedControl::Period::ON);
                    m_pBox->getRgbLedControl()->updateOutputValues(true);
                }
            }
			break;
		}
		case MSG_ID_GETBOXDATA_RESP: {
            if(m_pBox != nullptr && m_pSocketBoard == nullptr){
                JsonObject data = obj.getObject("data");
                uint32_t ownerID = data.getInt("owner");
                m_pBox->setOwner(ownerID);
                bool isLocked = data.getBoolean("isLocked");
                m_pBox->setIsLocked(isLocked);
                ESP_LOGD(LOG_TAG, "MSG_ID_GETBOXDATA_RESP received, ownerID = %d, isLocked = %d", ownerID, isLocked);

                if(m_pBox->getIsPendingOpenRequest() == true){
                    if(isLocked){
                        m_pBox->getRgbLedControl()->setColor(RgbLedControl::Color::RED);
                        m_pBox->getRgbLedControl()->setPeriod(RgbLedControl::Period::ON);
                        m_pBox->getRgbLedControl()->updateOutputValues(true);
                    }
                    else{
                        m_pBox->open();
                    }
                }
            }
			break;
		}
		case MSG_ID_STARTBOXCODEPARSING: {
            if(m_pBox != nullptr && m_pSocketBoard == nullptr){
                //m_pBox->getBoxCodeParser()->startParsing(m_pBox->getOwner());
            }
			break;
		}
		case MSG_ID_STOPBOXCODEPARSING: {
            if(m_pBox != nullptr && m_pSocketBoard == nullptr){
                //m_pBox->getBoxCodeParser()->stopParsing(m_pBox->getOwner());
            }
			break;
		}
		case MSG_ID_LOCKBOX: {
            if(m_pBox != nullptr && m_pSocketBoard == nullptr){
                JsonObject data = obj.getObject("data");
                uint32_t ownerID = data.getInt("owner");
                //m_pBox->setOwner(ownerID);    // HN-CHECK -> set box owner, maybe add response to server if boxcode is already set for that user?
                m_pBox->setIsLocked(true);
                ESP_LOGD(LOG_TAG, "MSG_ID_LOCKBOX received, ownerID = %d", ownerID);

                // send ACK to server to confirm box locked
                uint32_t refuid = obj.getInt("uid");
                CommHandlerSendData_t sendData;
                sendData.msgID = MSG_ID_ACK;
                sendData.value1 = refuid;
                addSendMessage(sendData);   // send MSG_ID_ACK to server

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
 * @brief Send a message to the Rewardi server
 */
bool CommHandler::sendData(CommHandlerSendData_t sendData){

    JsonObject obj = JSON::createObject();
    obj.setString("token", m_sessionToken);
    obj.setInt("uid", esp_random());

    switch(sendData.msgID){
        case MSG_ID_ACK: {
            obj.setInt("type", MSG_ID_ACK);
            JsonObject data = JSON::createObject();
            data.setInt("refuid", sendData.value1);
            obj.setObject("data", data);
            ESP_LOGD(LOG_TAG, "Send MSG_ID_ACK: %s", obj.toStringUnformatted().c_str());
            break;
        }
        case MSG_ID_NACK: {
            obj.setInt("type", MSG_ID_NACK);
            JsonObject data = JSON::createObject();
            data.setInt("errno", sendData.value2);
            data.setString("message", "ESP32 NACK");
            data.setInt("refuid", sendData.value1);
            obj.setObject("data", data);
            ESP_LOGD(LOG_TAG, "Send MSG_ID_NACK: %s", obj.toStringUnformatted().c_str());
            break;
        }
        case MSG_ID_PING: {
            sendEncodedRawData("ping", WS_OPCODE_PING);
            return true;
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
            JsonObject data = JSON::createObject();
            data.setInt("duration", sendData.value1);
            data.setInt("deltaTime", sendData.value2);
            data.setBoolean("isTimeout", sendData.flag1);
            obj.setObject("data", data);
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
 * @brief Add a message that is to be sent to the Rewardi server to the message queue
 */
bool CommHandler::addSendMessage(CommHandlerSendData_t sendData){
    if(xQueueSend(m_sendDataQueue,(void *)&sendData,(TickType_t )0)){       // todo: check if sendData is valid!
        return true;
    }
    return false;
}

/**
 * @brief Encodes the message data to Websocket protocol format and sends it to Rewardi server
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
    ESP_LOGD(LOG_TAG, "Send sendEncodedRawData: %s", str);
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
        frameBuffer[bufferIndex] = (str[i] ^ mask[i % 4]);  // payload in messages sent from client to server need to be masked according to Websocket specification
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
 * @brief Extract message payload from a received message in Websocket protocol format
 */
bool CommHandler::receiveData(){
	uint32_t bytesRead = 0;
	while(bytesRead == 0){
	    bytesRead = m_pSocket->receive(m_pReceiveBuffer, 512, false);
	}

	// print full receive buffer - DEBUG
	m_pReceiveBuffer[511] = '\0';
	ESP_LOGD(LOG_TAG, "received string = %s", &m_pReceiveBuffer[0]);
	ESP_LOGD(LOG_TAG, "byte0 = %d", m_pReceiveBuffer[0]);
	ESP_LOGD(LOG_TAG, "byte1 = %d", m_pReceiveBuffer[1]);
	ESP_LOGD(LOG_TAG, "byte2 = %d", m_pReceiveBuffer[2]);
	ESP_LOGD(LOG_TAG, "byte3 = %d", m_pReceiveBuffer[3]);

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
 * @brief Send Websocket close information to Rewardi server
 */
bool CommHandler::closeWebsocket(){
    sendEncodedRawData("", WS_OPCODE_CLOSE);
    m_isConnected = false;
    return true;
}

/**
 * @brief CommHandlerReceiveTask - used to receive messages from Rewardi server
 */
class CommHandlerReceiveTask: public Task {
public:
    CommHandlerReceiveTask(std::string name): Task(name, 16 * 1024) {
        m_pCommHandler = nullptr;
    };

private:
    CommHandler* m_pCommHandler; // Reference to the CommHandler

    /**
     * @brief Check for new data received from Rewardi server
     */
    void run(void* data) {
        m_pCommHandler = (CommHandler*) data;   // The passed in data is an instance of an CommHandler.

        while (true) {   // Loop forever.
            m_pCommHandler->receiveData();
        } // while
    } // run
}; // CommHandlerReceiveTask

/**
 * @brief CommHandlerSendTask - used to send messages to Rewardi server
 */
class CommHandlerSendTask: public Task {
public:
    CommHandlerSendTask(std::string name): Task(name, 16 * 1024) {
        m_pCommHandler = nullptr;
    };

private:
    CommHandler* m_pCommHandler; // Reference to the CommHandler

    /**
     * @brief Check if new data to send to Rewardi server is added to the message queue -> then send it
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
            if(xQueueReceive(m_pCommHandler->m_sendDataQueue,&sendData,pdMS_TO_TICKS(1000))){
                m_pCommHandler->sendData(sendData);  // send out data if there is something added to the queue
            }
        } // while
    } // run
}; // CommHandlerSendTask

/**
 * @brief CommHandlerHeartbeatTask - used to keep Websocket connection alive (ping/pong mechanism as specified in the Websocket specification)
 */
class CommHandlerHeartbeatTask: public Task {
public:
    CommHandlerHeartbeatTask(std::string name): Task(name, 16 * 1024) {
        m_pCommHandler = nullptr;
    };

private:
    CommHandler* m_pCommHandler; // Reference to the CommHandler

    /**
     * @brief Send a Websocket ping packet to the Rewardi server every 30s
     */
    void run(void* data) {
        m_pCommHandler = (CommHandler*) data;   // The passed in data is an instance of an CommHandler.
        CommHandlerSendData_t sendData;
        sendData.msgID = MSG_ID_PING;
        sendData.value1 = 0;
        sendData.value2 = 0;
        sendData.value3 = 0;
        sendData.flag1 = false;
        sendData.flag2 = false;

        while (true) {   // Loop forever.
            ESP_LOGD("CommHandlerHeartbeatTask", "Send hearbeat (ping)");
            m_pCommHandler->addSendMessage(sendData);
            vTaskDelay(pdMS_TO_TICKS(30000));
        } // while
    } // run
}; // CommHandlerSendTask

/**
 * @brief Start the CommHandler -> create the message queue for outgoing messages and then start the three
 *        communication tasks (one to send data, one to receive data and one to keep the connection alive)
 */
void CommHandler::start(){
    m_sendDataQueue = xQueueCreate(10,sizeof(CommHandlerSendData_t));
    if(!m_sendDataQueue){
        ESP_LOGD(LOG_TAG, "Failed to create sendDataQueue!");
    }

    m_pCommHandlerSendTask = new CommHandlerSendTask("CommHandlerSendTask");
    m_pCommHandlerSendTask->start(this);

    m_pCommHandlerReceiveTask = new CommHandlerReceiveTask("CommHandlerReceiveTask");
    m_pCommHandlerReceiveTask->start(this);

    m_pCommHandlerHeartbeatTask = new CommHandlerHeartbeatTask("CommHandlerHeartbeatTask");
    m_pCommHandlerHeartbeatTask->start(this);
}

/**
 * @brief Stop the CommHandler -> close the Websocket connection and stop the three communication tasks
 */
void CommHandler::stop(){
    closeWebsocket();
    m_pCommHandlerSendTask->stop();
    m_pCommHandlerReceiveTask->stop();
    m_pCommHandlerHeartbeatTask->stop();
}
