/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : ButtonDriver.cpp
 * Purpose    : Driver to get the push button input of the Rewardi Box
 ********************************************************************************************/

#include "ButtonDriver.h"

/**
 * @brief ButtonDriver constructor - needs pin number where push button is connected (switches pin to GND on button press)
 */
ButtonDriver::ButtonDriver(gpio_num_t buttonPin){
    m_buttonPin = buttonPin;
}

/**
 * @brief Set pin number
 */
void ButtonDriver::setPin(gpio_num_t buttonPin){
    m_buttonPin = buttonPin;
}

/**
 * @brief Get pin number
 */
gpio_num_t ButtonDriver::getPin(){
    return m_buttonPin;
}

/**
 * @brief Initialize ButtonDriver -> init the input pin
 */
void ButtonDriver::init(){
    gpio_set_direction(m_buttonPin, GPIO_MODE_INPUT);    // set gpio pin as input
    gpio_pullup_en(m_buttonPin);
    gpio_pulldown_dis(m_buttonPin);
}

/**
 * @brief Check if push button is pressed
 */
uint32_t ButtonDriver::getLevel(){
    return gpio_get_level(m_buttonPin);
}
