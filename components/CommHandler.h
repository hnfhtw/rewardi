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

class CommHandler {
public:
	CommHandler();
	void setBox(Box* pBox);
	Box* getBox();
	void setSocket(Socket* pSocket);
	Socket* getSocket();
	bool parseMessage(const char* message);
	bool sendLogin();
    bool sendEncodedData(char const *str, uint8_t opcode);
    bool receiveData();
private:
    Socket* m_pSocket;
	Box* m_pBox;
	uint8_t m_pReceiveBuffer[512];
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
#define RAND_MAX 256


#endif /* COMPONENTS_COMMHANDLER_H_ */
