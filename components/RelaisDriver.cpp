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
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;          //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;                //set pin as output mode
    io_conf.pin_bit_mask = (1<<m_relaisPin);          //bit mask of pin
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;    //enable pull-down resistor
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;       //disable pull-up resistor
    gpio_config(&io_conf);                          //configure GPIO with the given settings

    gpio_set_level(m_relaisPin, 0);
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
