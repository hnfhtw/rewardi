/*
 * RelaisDriver.h
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

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
