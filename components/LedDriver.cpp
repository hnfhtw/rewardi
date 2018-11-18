/*
 * LedDriver.cpp
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#include "LedDriver.h"

/**
 * @brief xxx
 */
LedDriver::LedDriver(gpio_num_t ledPin){
    m_ledPin = ledPin;
}

/**
 * @brief xxx
 */
void LedDriver::setPin(gpio_num_t ledPin){
    m_ledPin = ledPin;
}

/**
 * @brief xxx
 */
gpio_num_t LedDriver::getPin(){
    return m_ledPin;
}

/**
 * @brief xxx
 */
void LedDriver::init(){
    gpio_set_direction(m_ledPin, GPIO_MODE_OUTPUT);    // set gpio pin as output
    gpio_set_level(m_ledPin, 0);
    gpio_pullup_dis(m_ledPin);
    gpio_pulldown_en(m_ledPin);
}

/**
 * @brief xxx
 */
void LedDriver::switchOn(){
    gpio_set_level(m_ledPin, 1);
}

/**
 * @brief xxx
 */
void LedDriver::switchOff(){
    gpio_set_level(m_ledPin, 0);
}
