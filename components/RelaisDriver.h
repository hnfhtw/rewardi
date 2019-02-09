/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : RelaisDriver.h
 * Purpose    : Driver to switch the (monostable) relais of a Rewardi SocketBoard
 ********************************************************************************************/

#ifndef COMPONENTS_RELAISDRIVER_H_
#define COMPONENTS_RELAISDRIVER_H_

#include <driver/gpio.h>

class RelaisDriver {
public:
    RelaisDriver(gpio_num_t relaisPin);
    void setPin(gpio_num_t relaisPin);
    gpio_num_t getPin();
    void init();
    void switchOn();
    void switchOff();

private:
    gpio_num_t      m_relaisPin;
};

#endif /* COMPONENTS_RELAISDRIVER_H_ */
