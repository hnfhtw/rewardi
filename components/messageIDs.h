/*
 * messageIDs.h
 *
 *  Created on: 03.11.2018
 *      Author: HN
 */

#ifndef COMPONENTS_MESSAGEIDS_H_
#define COMPONENTS_MESSAGEIDS_H_

// ID 0 to 99 for general messages that are used everywhere (socket, box, backend)
#define MSG_ID_ACK                          0   // server <-> box/socket
#define MSG_ID_NACK                         1   // server <-> box/socket
#define MSG_ID_INIT                        10   // server -> box/socket

// ID 100 to 199 for box related messages
#define MSG_ID_REQUESTOPEN_RESP           100   // server -> box
#define MSG_ID_LOCKBOX                    101   // server -> box
#define MSG_ID_GETBOXDATA_RESP            102   // server -> box
#define MSG_ID_STARTBOXCODEPARSING        103   // server -> box
#define MSG_ID_STOPBOXCODEPARSING         104   // server -> box

#define MSG_ID_REQUESTOPEN                150   // server <- box
#define MSG_ID_GETBOXDATA                 151   // server <- box
#define MSG_ID_SETBOXEVENT                152   // server <- box

// ID 200 to 299 for socket related messages
#define MSG_ID_ACTIVATESOCKET             200   // server -> socket
#define MSG_ID_DEACTIVATESOCKET           201   // server -> socket

#define MSG_ID_SETSOCKETEVENT             250   // server <- socket

#endif /* COMPONENTS_MESSAGEIDS_H_ */
