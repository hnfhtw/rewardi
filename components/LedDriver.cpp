/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : LedDriver.cpp
 * Purpose    : Driver to switch an LED (three of these drivers are used for the RGB LED of the Rewardi Box)
 ********************************************************************************************/

#include "LedDriver.h"

/**
 * @brief LedDriver constructor - needs pin number where LED is connected
 */
LedDriver::LedDriver(gpio_num_t ledPin){
    m_ledPin = ledPin;
}

/**
 * @brief Set pin number
 */
void LedDriver::setPin(gpio_num_t ledPin){
    m_ledPin = ledPin;
}

/**
 * @brief Get pin number
 */
gpio_num_t LedDriver::getPin(){
    return m_ledPin;
}

/**
 * @brief Initialize LedDriver -> init output pin
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
 * @brief Switch on LED
 */
void LedDriver::switchOn(){
    gpio_set_level(m_ledPin, 1);
}

/**
 * @brief Switch off LED
 */
void LedDriver::switchOff(){
    gpio_set_level(m_ledPin, 0);
}
