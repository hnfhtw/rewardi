/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : LedDriver.h
 * Purpose    : Driver to switch an LED (three of these drivers are used for the RGB LED of the Rewardi Box)
 ********************************************************************************************/

#ifndef COMPONENTS_LEDDRIVER_H_
#define COMPONENTS_LEDDRIVER_H_

#include <driver/gpio.h>

class LedDriver {
public:
    LedDriver(gpio_num_t ledPin);
    void setPin(gpio_num_t ledPin);
    gpio_num_t getPin();
    void init();
    void switchOn();
    void switchOff();

private:
    gpio_num_t      m_ledPin;
};

#endif /* COMPONENTS_LEDDRIVER_H_ */
