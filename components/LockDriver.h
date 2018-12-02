/*
 * LockDriver.h
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#ifndef COMPONENTS_LOCKDRIVER_H_
#define COMPONENTS_LOCKDRIVER_H_

#include <FreeRTOS.h>
#include <driver/gpio.h>
#include <freertos/timers.h>

class LockDriver {
public:
    LockDriver(gpio_num_t lockPin);
    ~LockDriver();
    void setPin(gpio_num_t lockPin);
    gpio_num_t getPin();
    void init();
    void switchOn();
    void switchOff();
    static void timeout(TimerHandle_t xTimer);
private:
    gpio_num_t      m_lockPin;
    TimerHandle_t   m_hTimeout;
};

#endif /* COMPONENTS_LOCKDRIVER_H_ */
