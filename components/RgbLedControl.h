/*
 * RgbLedControl.h
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#ifndef COMPONENTS_RGBLEDCONTROL_H_
#define COMPONENTS_RGBLEDCONTROL_H_

#include "LedDriver.h"

class RgbLedControl {
public:
    enum Color{BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA};
    //RgbLedControl(LedDriver* pLedRed, LedDriver* pLedGreen, LedDriver* pLedBlue);
    RgbLedControl(gpio_num_t ledRedPin, gpio_num_t ledGreenPin, gpio_num_t ledBluePin);
    void init();
    void setColor(RgbLedControl::Color color);

private:
    LedDriver*      m_pLedRed;
    LedDriver*      m_pLedGreen;
    LedDriver*      m_pLedBlue;
};

#endif /* COMPONENTS_RGBLEDCONTROL_H_ */
