/*
 * ButtonDriver.cpp
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#include "ButtonDriver.h"

/**
 * @brief xxx
 */
ButtonDriver::ButtonDriver(gpio_num_t buttonPin){
    m_buttonPin = buttonPin;
}

/**
 * @brief xxx
 */
void ButtonDriver::setPin(gpio_num_t buttonPin){
    m_buttonPin = buttonPin;
}

/**
 * @brief xxx
 */
gpio_num_t ButtonDriver::getPin(){
    return m_buttonPin;
}

/**
 * @brief xxx
 */
void ButtonDriver::init(){
    gpio_set_direction(m_buttonPin, GPIO_MODE_INPUT);    // set gpio pin as input
    gpio_pullup_en(m_buttonPin);
    gpio_pulldown_dis(m_buttonPin);
}

/**
 * @brief xxx
 */
uint32_t ButtonDriver::getLevel(){
    return gpio_get_level(m_buttonPin);
}
