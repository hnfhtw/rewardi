/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : RgbLedControl.h
 * Purpose    : Driver to control the RGB LED of a Rewardi Box
 ********************************************************************************************/

#ifndef COMPONENTS_RGBLEDCONTROL_H_
#define COMPONENTS_RGBLEDCONTROL_H_

#include <FreeRTOS.h>
#include <freertos/timers.h>
#include "LedDriver.h"

class RgbLedControl {
public:
    enum Color{WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA};
    enum Period{ON, MS_1000, MS_2000, OFF};
    RgbLedControl(gpio_num_t ledRedPin, gpio_num_t ledGreenPin, gpio_num_t ledBluePin);
    ~RgbLedControl();
    void init();
    void start();
    void setColor(RgbLedControl::Color color);
    void setPeriod(RgbLedControl::Period period);
    Color getColor();
    Period getPeriod();
    void updateOutputValues(bool isOutputOn);
    static void timeout(TimerHandle_t xTimer);
    uint8_t         m_time;
private:
    LedDriver*      m_pLedRed;
    LedDriver*      m_pLedGreen;
    LedDriver*      m_pLedBlue;
    TimerHandle_t   m_hBlinkTimer;
    Color           m_color;
    Period          m_period;

};

#endif /* COMPONENTS_RGBLEDCONTROL_H_ */
