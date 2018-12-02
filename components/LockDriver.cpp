/*
 * LockDriver.cpp
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#include "LockDriver.h"
#include <esp_log.h>
#include <map>
#include "driver/gpio.h"

#define MAX_LOCK_TIME_MS   5000

static const char* LOG_TAG = "LockDriver";
static std::map<void*, LockDriver*> lockDriverMap;


/**
 * @brief xxx
 */
LockDriver::LockDriver(gpio_num_t lockPin){
    m_lockPin = lockPin;
    m_hTimeout = nullptr;
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
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;          //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;                //set pin as output mode
    io_conf.pin_bit_mask = (1<<m_lockPin);          //bit mask of pin
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;    //enable pull-down resistor
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;       //disable pull-up resistor
    gpio_config(&io_conf);                          //configure GPIO with the given settings

    gpio_set_level(m_lockPin, 0);

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


