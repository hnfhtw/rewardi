/*
 * LockDriver.cpp
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#include "LockDriver.h"
#include <esp_log.h>
#include <map>

#define MAX_LOCK_TIME_MS   5000

static const char* LOG_TAG = "LockDriver";
static std::map<void*, LockDriver*> lockDriverMap;

/**
 * @brief xxx
 */
LockDriver::LockDriver(){
}

/**
 * @brief xxx
 */
LockDriver::LockDriver(gpio_num_t lockPin){
    m_lockPin = lockPin;
}

LockDriver::~LockDriver(){
    ::xTimerDelete(m_hTimeout, portMAX_DELAY);
    lockDriverMap.erase(m_hTimeout);
}

/**
 * @brief xxx
 */
void LockDriver::setPin(gpio_num_t lockPin){
    m_lockPin = lockPin;
}

/**
 * @brief xxx
 */
gpio_num_t LockDriver::getPin(){
    return m_lockPin;
}

/**
 * @brief xxx
 */
void LockDriver::init(){
    gpio_set_direction(m_lockPin, GPIO_MODE_OUTPUT);    // set gpio pin as output
    gpio_set_level(m_lockPin, 0);
    gpio_pullup_dis(m_lockPin);
    gpio_pulldown_en(m_lockPin);

    m_hTimeout = xTimerCreate("SysTick",  pdMS_TO_TICKS(MAX_LOCK_TIME_MS), pdFALSE, NULL, (TimerCallbackFunction_t) &LockDriver::timeout);    // create and initialize 10s timeout timer
    lockDriverMap.insert(std::make_pair(m_hTimeout, this));
}

/**
 * @brief xxx
 */
void LockDriver::switchOn(){
    ESP_LOGD(LOG_TAG, "switchOn");
    xTimerStart(m_hTimeout, 0);   // start 10s timeout timer
    gpio_set_level(m_lockPin, 1);
}

/**
 * @brief xxx
 */
void LockDriver::switchOff(){
    ESP_LOGD(LOG_TAG, "switchOff, m_lockPin = %d", m_lockPin);
    gpio_set_level(m_lockPin, 0);
}

/**
 * @brief xxx
 */
void LockDriver::timeout(TimerHandle_t xTimer){
    LockDriver* lockDriver = lockDriverMap.at(xTimer);
    lockDriver->switchOff();
}


