/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : StandbyControl.h
 * Purpose    : Module that sends ESP32 to deep sleep mode if requested, and wakes controller up
 *              by push button press (in case of Rewardi Box)
 ********************************************************************************************/

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
