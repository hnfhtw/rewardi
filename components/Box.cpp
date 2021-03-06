/********************************************************************************************
 * Project    : Rewardi
 * Created on : 22.10.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : Box.cpp
 * Purpose    : Represents a Rewardi Box
 ********************************************************************************************/

#include <esp_log.h>
#include <map>
#include "Box.h"
#include "messageIDs.h"
#include "nvs_flash.h"

#define BOX_AWAKE_TIME_MS   60000

#define DEVICE_DATA_PARTITION           "device_data"
#define DEVICE_DATA_NAMESPACE           "device_data"
#define DEVICE_DATA_KEY_USER0_ID        "user0_id"
#define DEVICE_DATA_KEY_USER0_BOXCODE   "user0_boxcode"
#define DEVICE_DATA_KEY_USER1_ID        "user1_id"
#define DEVICE_DATA_KEY_USER1_BOXCODE   "user1_boxcode"
#define DEVICE_DATA_KEY_USER2_ID        "user2_id"
#define DEVICE_DATA_KEY_USER2_BOXCODE   "user2_boxcode"
#define DEVICE_DATA_KEY_USER3_ID        "user3_id"
#define DEVICE_DATA_KEY_USER3_BOXCODE   "user3_boxcode"
#define DEVICE_DATA_KEY_USER4_ID        "user4_id"
#define DEVICE_DATA_KEY_USER4_BOXCODE   "user4_boxcode"

static const char* LOG_TAG = "Box";
static std::map<void*, Box*> boxMap;

/**
 * @brief Create a Box instance.
 */
Box::Box(gpio_num_t lockPin, gpio_num_t ledRedPin, gpio_num_t ledGreenPin, gpio_num_t ledBluePin, gpio_num_t buttonPin){
	m_pOwner			= nullptr;
    m_pRgbLedControl    = new RgbLedControl(ledRedPin, ledGreenPin, ledBluePin);    // create a RgbLedControl object that controls the RGB LED
    m_pLockDriver       = new LockDriver(lockPin, this);                            // create a LockDriver object that controls the lock
    m_pBoxCodeParser    = new BoxCodeParser(buttonPin, this);                       // create a BoxCodeParser object that parses user inputs on the push button
    m_pCommHandler      = nullptr;
    m_userListSize      = 0;
    m_isLocked          = false;
    m_hTimeout          = 0;
    m_pSysControl       = nullptr;
    m_isPendingOpenRequest = false;
}

/**
 * @brief Box destructor
 */
Box::~Box(){
    ::xTimerDelete(m_hTimeout, portMAX_DELAY);
    boxMap.erase(m_hTimeout);
}

/**
 * @brief Initialize Box
 */
void Box::init(){
    m_pLockDriver->init();
    m_pRgbLedControl->init();
    m_pBoxCodeParser->init();
    readUserListData();
    m_hTimeout = xTimerCreate("boxAwakeTimer",  pdMS_TO_TICKS(BOX_AWAKE_TIME_MS), pdFALSE, NULL, (TimerCallbackFunction_t) &Box::timeout);    // create and initialize awake timeout timer -> box will go to deep sleep after this time
    boxMap.insert(std::make_pair(m_hTimeout, this));

    m_pRgbLedControl->setColor(RgbLedControl::Color::WHITE);
    m_pRgbLedControl->setPeriod(RgbLedControl::Period::MS_1000);
    m_pRgbLedControl->updateOutputValues(true);
    m_pRgbLedControl->start();
}


/**
 * @brief Start Box (called when server connection was established successfully)
 */
void Box::start(){
    CommHandlerSendData_t sendData;         // obtain box owner and isLocked status at startup
    sendData.msgID = MSG_ID_GETBOXDATA;
    m_pCommHandler->addSendMessage(sendData);   // send MSG_ID_REQUESTOPEN to server

    m_pBoxCodeParser->start();  // start box code parser

    xTimerStart(m_hTimeout, 0);   // start awake timeout timer

    m_pRgbLedControl->setColor(RgbLedControl::Color::YELLOW);
    m_pRgbLedControl->setPeriod(RgbLedControl::Period::ON);
    m_pRgbLedControl->updateOutputValues(true);
}


/**
 * @brief Set the current Box owner (ownerID is received from server)
 */
void Box::setOwner(uint32_t ownerID){
    if(ownerID == m_userList[0]->getUserID()){
        return;     // correct owner already set
    }

    if(m_userListSize == 0){    // if user list is empty - add new user to position 0 and set user as current owner
        m_userList[0] = new User(ownerID);
        m_pOwner = m_userList[0];
        ++m_userListSize;
    }
    else{   // search if new user is already in list
        uint8_t i;
        bool userFound = false;
        for(i = 0; i<m_userListSize; ++i){
            if(m_userList[i]->getUserID() == ownerID){   // new user is in list at position i
                userFound = true;
                break;
            }
        }
        if(userFound){  // user was found in list at position i
            User* temp = m_userList[i];  // save user object - it will be relocated to position 0
            for(uint8_t j = i; j>0; --j){
                m_userList[j] = m_userList[j-1];    // move all users one position down
            }
            m_userList[0] = temp;  // set current user to position 0
            m_pOwner = m_userList[0];
        }
        else{
            if(m_userListSize == BOX_MAX_NR_OF_USERS){   // list is full - delete user on last position
                delete m_userList[BOX_MAX_NR_OF_USERS-1];
                --m_userListSize;
            }
            for(uint8_t i = m_userListSize; i>0; --i){
                m_userList[i] = m_userList[i-1];    // move all users one position down
            }
            m_userList[0] = new User(ownerID);
            m_pOwner = m_userList[0];
            ++m_userListSize;
        }
    }
    writeUserListData();
}

/**
 * @brief Set pointer to CommHandler object
 */
void Box::setCommHandler(CommHandler* pCommHandler){
    m_pCommHandler = pCommHandler;
}

/**
 * @brief Set lock information
 */
void Box::setIsLocked(bool isLocked){
    m_isLocked = isLocked;
}

/**
 * @brief Set flag that indicates if a open request was sent to the server and a response is not yet received
 */
void Box::setIsPendingOpenRequest(bool isPendingOpenRequest){
    m_isPendingOpenRequest = isPendingOpenRequest;
}

/**
 * @brief Get current Box owner
 */
User* Box::getOwner(){
	return m_pOwner;
}

/**
 * @brief Get pointer to Box LockDriver object
 */
LockDriver* Box::getLockDriver(){
    return m_pLockDriver;
}

/**
 * @brief Get pointer to Box RgbLedControl object
 */
RgbLedControl* Box::getRgbLedControl(){
    return m_pRgbLedControl;
}

/**
 * @brief Get pointer to Box BoxCodeParser object
 */
BoxCodeParser* Box::getBoxCodeParser(){
    return m_pBoxCodeParser;
}

/**
 * @brief Get Box lock status
 */
bool Box::getIsLocked(){
    return m_isLocked;
}

/**
 * @brief Check if a pending Box open request is active (-> answer from server not yet received)
 */
bool Box::getIsPendingOpenRequest(){
    return m_isPendingOpenRequest;
}

/**
 * @brief Called from BoxCodeParser if a code was entered by the user with the push button -> send open request to server if correct box code was entered
 */
void Box::requestBoxOpen(uint8_t boxCode){
    if(boxCode == m_pOwner->getBoxCode()){  // send open box request to server if the box code of the current user (owner) was entered
        CommHandlerSendData_t sendData;
        sendData.msgID = MSG_ID_REQUESTOPEN;
        m_pCommHandler->addSendMessage(sendData);   // send MSG_ID_REQUESTOPEN to server
    }
    else{
        m_pRgbLedControl->setColor(RgbLedControl::Color::RED);
        m_pRgbLedControl->setPeriod(RgbLedControl::Period::ON);
        m_pRgbLedControl->updateOutputValues(true);
    }
}

/**
 * @brief Sends a request to the server to update box data
 */
void Box::updateBoxData(){
    CommHandlerSendData_t sendData;
    sendData.msgID = MSG_ID_GETBOXDATA;
    m_pCommHandler->addSendMessage(sendData);   // send MSG_ID_GETBOXDATA to server
}

/**
 * @brief Open the box -> user then has 5s time to open the Box, afterwards the LockDriver will turn-off the lock again (to avoid overheating)
 */
void Box::open(){
    m_pLockDriver->switchOn();
}

/**
 * @brief Read the last 5 users and their 5-digit box codes from Flash
 */
bool Box::readUserListData(){
    bool rtnVal = false;

    esp_err_t err = nvs_flash_init_partition(DEVICE_DATA_PARTITION);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_flash_init_partition() error = %d", err);
        return rtnVal;
    }

    nvs_handle hNVS;
    err = nvs_open_from_partition(DEVICE_DATA_PARTITION, DEVICE_DATA_NAMESPACE, NVS_READWRITE, &hNVS);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_open_from_partition() error = %d", err);
        return rtnVal;
    }


    uint32_t user0_ID = 0;
    uint8_t user0_boxcode = 0;
    err = nvs_get_u32(hNVS, DEVICE_DATA_KEY_USER0_ID, &user0_ID);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_u32() error = %d", err);
        return rtnVal;
    }
    err = nvs_get_u8(hNVS, DEVICE_DATA_KEY_USER0_BOXCODE, &user0_boxcode);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_u8() error = %d", err);
        return rtnVal;
    }

    uint32_t user1_ID = 0;
    uint8_t user1_boxcode = 0;
    err = nvs_get_u32(hNVS, DEVICE_DATA_KEY_USER1_ID, &user1_ID);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_u32() error = %d", err);
        return rtnVal;
    }
    err = nvs_get_u8(hNVS, DEVICE_DATA_KEY_USER1_BOXCODE, &user1_boxcode);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_u8() error = %d", err);
        return rtnVal;
    }

    uint32_t user2_ID = 0;
    uint8_t user2_boxcode = 0;
    err = nvs_get_u32(hNVS, DEVICE_DATA_KEY_USER2_ID, &user2_ID);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_u32() error = %d", err);
        return rtnVal;
    }
    err = nvs_get_u8(hNVS, DEVICE_DATA_KEY_USER2_BOXCODE, &user2_boxcode);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_u8() error = %d", err);
        return rtnVal;
    }

    uint32_t user3_ID = 0;
    uint8_t user3_boxcode = 0;
    err = nvs_get_u32(hNVS, DEVICE_DATA_KEY_USER3_ID, &user3_ID);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_u32() error = %d", err);
        return rtnVal;
    }
    err = nvs_get_u8(hNVS, DEVICE_DATA_KEY_USER3_BOXCODE, &user3_boxcode);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_u8() error = %d", err);
        return rtnVal;
    }

    uint32_t user4_ID = 0;
    uint8_t user4_boxcode = 0;
    err = nvs_get_u32(hNVS, DEVICE_DATA_KEY_USER4_ID, &user4_ID);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_u32() error = %d", err);
        return rtnVal;
    }
    err = nvs_get_u8(hNVS, DEVICE_DATA_KEY_USER4_BOXCODE, &user4_boxcode);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_u8() error = %d", err);
        return rtnVal;
    }

    m_userList[0] = new User(user0_ID, user0_boxcode);
    m_userList[1] = new User(user1_ID, user1_boxcode);
    m_userList[2] = new User(user2_ID, user2_boxcode);
    m_userList[3] = new User(user3_ID, user3_boxcode);
    m_userList[4] = new User(user4_ID, user4_boxcode);

    m_userListSize = 5;
    m_pOwner = m_userList[0];

    nvs_close(hNVS);

    return rtnVal;
}

/**
 * @brief Write the last 5 users and their 5-digit box codes from Flash
 */
bool Box::writeUserListData(){
    bool rtnVal = false;

    esp_err_t err = nvs_flash_init_partition(DEVICE_DATA_PARTITION);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_flash_init_partition() error = %d", err);
        return rtnVal;
    }

    nvs_handle hNVS;
    err = nvs_open_from_partition(DEVICE_DATA_PARTITION, DEVICE_DATA_NAMESPACE, NVS_READWRITE, &hNVS);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_open_from_partition() error = %d", err);
        return rtnVal;
    }

    err = nvs_set_u32(hNVS, DEVICE_DATA_KEY_USER0_ID, m_userList[0]->getUserID());
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_set_u32() error = %d", err);
        return rtnVal;
    }
    err = nvs_set_u8(hNVS, DEVICE_DATA_KEY_USER0_BOXCODE, m_userList[0]->getBoxCode());
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_set_u8() error = %d", err);
        return rtnVal;
    }

    err = nvs_set_u32(hNVS, DEVICE_DATA_KEY_USER1_ID, m_userList[1]->getUserID());
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_set_u32() error = %d", err);
        return rtnVal;
    }
    err = nvs_set_u8(hNVS, DEVICE_DATA_KEY_USER1_BOXCODE, m_userList[1]->getBoxCode());
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_set_u8() error = %d", err);
        return rtnVal;
    }

    err = nvs_set_u32(hNVS, DEVICE_DATA_KEY_USER2_ID, m_userList[2]->getUserID());
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_set_u32() error = %d", err);
        return rtnVal;
    }
    err = nvs_set_u8(hNVS, DEVICE_DATA_KEY_USER2_BOXCODE, m_userList[2]->getBoxCode());
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_set_u8() error = %d", err);
        return rtnVal;
    }

    err = nvs_set_u32(hNVS, DEVICE_DATA_KEY_USER3_ID, m_userList[3]->getUserID());
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_set_u32() error = %d", err);
        return rtnVal;
    }
    err = nvs_set_u8(hNVS, DEVICE_DATA_KEY_USER3_BOXCODE, m_userList[3]->getBoxCode());
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_set_u8() error = %d", err);
        return rtnVal;
    }

    err = nvs_set_u32(hNVS, DEVICE_DATA_KEY_USER4_ID, m_userList[4]->getUserID());
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_set_u32() error = %d", err);
        return rtnVal;
    }
    err = nvs_set_u8(hNVS, DEVICE_DATA_KEY_USER4_BOXCODE, m_userList[4]->getBoxCode());
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_set_u8() error = %d", err);
        return rtnVal;
    }

    nvs_close(hNVS);

    return rtnVal;
}

/**
 * @brief Called from BoxCodeParser if the user has entered a new 5-digit box code
 */
void Box::setBoxCode(uint8_t boxCode){
    ESP_LOGD(LOG_TAG, "New boxCode set = %d", boxCode);
    m_pOwner->setBoxCode(boxCode);

    writeUserListData();
}

/**
 * @brief Sleep mode timer -> when it expires this method is called and the flag that avoids entering deep sleep mode is reset
 */
void Box::timeout(TimerHandle_t xTimer){
    Box* pBox =boxMap.at(xTimer);
    pBox->m_pSysControl->setStayAwake(false);
}


/**
 * @brief Avoid entering deep sleep mode by this method -> it resets the 60s "stay awake timer" -> called on every push button press
 */
void Box::stayAwake(){
    xTimerReset(m_hTimeout, 0);   // reset awake timeout timer
}



