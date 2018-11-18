/*
 * StandbyControl.h
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#ifndef COMPONENTS_STANDBYCONTROL_H_
#define COMPONENTS_STANDBYCONTROL_H_

#include <driver/gpio.h>

class StandbyControl {
public:
    StandbyControl(gpio_num_t wakeupPin);
    void init();
    void enterSleepMode();
private:
    gpio_num_t      m_wakeupPin;
};

#endif /* COMPONENTS_STANDBYCONTROL_H_ */
