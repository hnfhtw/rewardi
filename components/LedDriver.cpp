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
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;          //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;                //set pin as output mode
    io_conf.pin_bit_mask = (1<<m_ledPin);           //bit mask of pin
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;    //enable pull-down resistor
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;       //disable pull-up resistor
    gpio_config(&io_conf);                          //configure GPIO with the given settings

    gpio_set_level(m_ledPin, 0);
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
