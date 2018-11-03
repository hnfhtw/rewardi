/*
 * messageIDs.h
 *
 *  Created on: 03.11.2018
 *      Author: HN
 */

#ifndef COMPONENTS_MESSAGEIDS_H_
#define COMPONENTS_MESSAGEIDS_H_

// ID 100 to 199 for general messages that are used everywhere (socket, box, backend)
#define MSG_ID_LOGIN                      100
#define MSG_ID_LOGIN_RESP                 101
#define MSG_ID_ACK                        102
#define MSG_ID_NACK                       103
#define MSG_ID_GETUSERDATA                104
#define MSG_ID_GETUSERDATA_RESP           105
#define MSG_ID_SETBACKENDURL              199

// ID 200 to 299 for socket related messages
#define MSG_ID_GETSOCKETID                200
#define MSG_ID_GETSOCKETID_RESP           201
#define MSG_ID_SETSOCKETID                202 // response = MSG_ID_ACK or MSG_ID_NACK message
#define MSG_ID_ADDSOCKET                  203
#define MSG_ID_ADDSOCKET_RESP             204
#define MSG_ID_GETSOCKETDATA              205
#define MSG_ID_GETSOCKETDATA_RESP         206
#define MSG_ID_SETSOCKETEVENT             207 // response = MSG_ID_ACK or MSG_ID_NACK message
#define MSG_ID_SETSOCKETDATA              208 // response = MSG_ID_ACK or MSG_ID_NACK message
#define MSG_ID_ACTIVATESOCKET             209 // response = MSG_ID_ACK or MSG_ID_NACK message
#define MSG_ID_DEACTIVATESOCKET           210
#define MSG_ID_DEACTIVATESOCKET_RESP      211

// ID 300 to 399 for box related messages
#define MSG_ID_GETBOXID                   300
#define MSG_ID_GETBOXID_RESP              301
#define MSG_ID_SETBOXID                   302 // response = MSG_ID_ACK or MSG_ID_NACK message
#define MSG_ID_ADDBOX                     303
#define MSG_ID_ADDBOX_RESP                304
#define MSG_ID_GETBOXDATA                 305
#define MSG_ID_GETBOXDATA_RESP            306
#define MSG_ID_SETBOXEVENT                307 // response = MSG_ID_ACK or MSG_ID_NACK message
#define MSG_ID_SETBOXDATA                 308 // response = MSG_ID_ACK or MSG_ID_NACK message
#define MSG_ID_SETOWNER                   309 // response = MSG_ID_BOXCODEPRESENT or MSG_ID_NACK message
#define MSG_ID_BOXCODEPRESENT             310
#define MSG_ID_STARTBOXCODEPARSING        312 // response = MSG_ID_ACK or MSG_ID_NACK message
#define MSG_ID_STOPBOXCODEPARSING         313 // response = MSG_ID_ACK or MSG_ID_NACK message
#define MSG_ID_LOCKBOX                    314 // response = MSG_ID_ACK or MSG_ID_NACK message
#define MSG_ID_BOXLOCKED                  315 // response = MSG_ID_ACK or MSG_ID_NACK message

#endif /* COMPONENTS_MESSAGEIDS_H_ */
