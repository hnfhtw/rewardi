/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : ButtonDriver.h
 * Purpose    : Driver to get the push button input of the Rewardi Box
 ********************************************************************************************/

#ifndef COMPONENTS_BUTTONDRIVER_H_
#define COMPONENTS_BUTTONDRIVER_H_

#include <driver/gpio.h>

class ButtonDriver {
public:
    ButtonDriver(gpio_num_t buttonPin);
    void setPin(gpio_num_t buttonPin);
    gpio_num_t getPin();
    void init();
    uint32_t getLevel();

private:
    gpio_num_t      m_buttonPin;
};

#endif /* COMPONENTS_BUTTONDRIVER_H_ */
