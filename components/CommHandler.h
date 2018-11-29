/*
 * CommHandler.h
 *
 *  Created on: 03.11.2018
 *      Author: HN
 */

#ifndef COMPONENTS_COMMHANDLER_H_
#define COMPONENTS_COMMHANDLER_H_

#include <Socket.h>
#include "Box.h"
#include "SocketBoard.h"
#include "freertos/queue.h"

class CommHandlerReceiveTask;
class CommHandlerSendTask;

typedef struct _sendData_ {
    uint32_t  msgID;
    uint32_t value1;
    uint32_t value2;
    uint32_t value3;
    bool      flag1;
    bool      flag2;
} CommHandlerSendData_t;

class SocketBoard;  // forward declaration to avoid error caused by recursive inclusion (SocketBoard.h includes CommHandler.h and vice versa)
class Box;          // forward declaration to avoid error caused by recursive inclusion (Box.h includes CommHandler.h and vice versa)

class CommHandler {
public:
	CommHandler();
	//void setSysControl(SysControl* pSysControl);
	//SysControl* getSysControl();
	void setBox(Box* pBox);
	Box* getBox();
    void setSocketBoard(SocketBoard* pSocketBoard);
    SocketBoard* getSocketBoard();
	void setSocket(Socket* pSocket);
	Socket* getSocket();
    bool addSendMessage(CommHandlerSendData_t data);
    bool closeWebsocket();
    void start();

private:
    friend class CommHandlerReceiveTask;
    friend class CommHandlerSendTask;
    Socket* m_pSocket;
	Box* m_pBox;
	SocketBoard* m_pSocketBoard;
	//SysControl* m_pSysControl;
	uint8_t m_pReceiveBuffer[512];
	std::string m_sessionToken;
	QueueHandle_t m_sendDataQueue;
	bool sendData(CommHandlerSendData_t data);
    bool sendEncodedRawData(char const *str, uint8_t opcode);
    bool receiveData();
    bool parseMessage(const char* message);
};

// WebSocket protocol constants
// First byte
#define WS_FIN            0x80
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02
#define WS_OPCODE_CLOSE   0x08
#define WS_OPCODE_PING    0x09
#define WS_OPCODE_PONG    0x0a
// Second byte
#define WS_MASK           0x80
#define WS_SIZE16         126
#define WS_SIZE64         127

#endif /* COMPONENTS_COMMHANDLER_H_ */
