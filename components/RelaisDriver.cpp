/*
 * RelaisDriver.cpp
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#include "RelaisDriver.h"

/**
 * @brief xxx
 */
RelaisDriver::RelaisDriver(){
}

/**
 * @brief xxx
 */
RelaisDriver::RelaisDriver(gpio_num_t relaisPin){
    m_relaisPin = relaisPin;
}

/**
 * @brief xxx
 */
void RelaisDriver::setPin(gpio_num_t relaisPin){
    m_relaisPin = relaisPin;
}

/**
 * @brief xxx
 */
gpio_num_t RelaisDriver::getPin(){
    return m_relaisPin;
}

/**
 * @brief xxx
 */
void RelaisDriver::init(){
    gpio_set_direction(m_relaisPin, GPIO_MODE_OUTPUT);    // set gpio pin as output
    gpio_set_level(m_relaisPin, 0);
    gpio_pullup_dis(m_relaisPin);
    gpio_pulldown_en(m_relaisPin);
}

/**
 * @brief xxx
 */
void RelaisDriver::switchOn(){
    gpio_set_level(m_relaisPin, 1);
}

/**
 * @brief xxx
 */
void RelaisDriver::switchOff(){
    gpio_set_level(m_relaisPin, 0);
}
