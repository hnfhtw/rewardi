/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : LockDriver.cpp
 * Purpose    : Driver for to switch the electromagnetic lock of the Rewardi Box
 ********************************************************************************************/

#include "LockDriver.h"
#include <esp_log.h>
#include <map>
#include "driver/gpio.h"

#define MAX_LOCK_TIME_MS   5000

static const char* LOG_TAG = "LockDriver";
static std::map<void*, LockDriver*> lockDriverMap;

/**
 * @brief LockDriver constructor -> needs pin number where lock is connected
 */
LockDriver::LockDriver(gpio_num_t lockPin, Box* pBox){
    m_lockPin = lockPin;
    m_hTimeout = nullptr;
    m_pBox = pBox;
}

/**
 * @brief Destructor
 */
LockDriver::~LockDriver(){
    ::xTimerDelete(m_hTimeout, portMAX_DELAY);
    lockDriverMap.erase(m_hTimeout);
}

/**
 * @brief Set output pin
 */
void LockDriver::setPin(gpio_num_t lockPin){
    m_lockPin = lockPin;
}

/**
 * @brief Get output pin
 */
gpio_num_t LockDriver::getPin(){
    return m_lockPin;
}

/**
 * @brief Initialize LockDriver -> init output pin, create a timer (to switch off lock after 5s automatically) and map the LockDriver object to this timer
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
 * @brief Switch on the lock (that means the box can be opened when the lock is powered / switched on)
 */
void LockDriver::switchOn(){
    ESP_LOGD(LOG_TAG, "switchOn");
    xTimerStart(m_hTimeout, 0);   // start 5s timeout timer
    gpio_set_level(m_lockPin, 1);
    m_pBox->getRgbLedControl()->setColor(RgbLedControl::Color::GREEN);
    m_pBox->getRgbLedControl()->setPeriod(RgbLedControl::Period::ON);
    m_pBox->getRgbLedControl()->updateOutputValues(true);
}

/**
 * @brief Switch off the lock
 */
void LockDriver::switchOff(){
    ESP_LOGD(LOG_TAG, "switchOff, m_lockPin = %d", m_lockPin);
    gpio_set_level(m_lockPin, 0);
    m_pBox->getRgbLedControl()->setColor(RgbLedControl::Color::YELLOW);
    m_pBox->getRgbLedControl()->setPeriod(RgbLedControl::Period::ON);
    m_pBox->getRgbLedControl()->updateOutputValues(true);
}

/**
 * @brief The lock is switched on for 5s - afterwards this timeout is called and the lock is switched off again
 */
void LockDriver::timeout(TimerHandle_t xTimer){
    LockDriver* lockDriver = lockDriverMap.at(xTimer);
    lockDriver->switchOff();
}


