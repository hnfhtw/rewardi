/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : RelaisDriver.cpp
 * Purpose    : Driver to switch the (monostable) relais of a Rewardi SocketBoard
 ********************************************************************************************/

#include "RelaisDriver.h"

/**
 * @brief RelaisDriver constructor - needs the output pin where the relais is connected
 */
RelaisDriver::RelaisDriver(gpio_num_t relaisPin){
    m_relaisPin = relaisPin;
}

/**
 * @brief Set the output pin
 */
void RelaisDriver::setPin(gpio_num_t relaisPin){
    m_relaisPin = relaisPin;
}

/**
 * @brief Get the output pin
 */
gpio_num_t RelaisDriver::getPin(){
    return m_relaisPin;
}

/**
 * @brief Initialize the RelaisDriver -> init the output pin
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
 * @brief Switch (monostable) relais on
 */
void RelaisDriver::switchOn(){
    gpio_set_level(m_relaisPin, 1);
}

/**
 * @brief Switch (monostable) relais off
 */
void RelaisDriver::switchOff(){
    gpio_set_level(m_relaisPin, 0);
}
