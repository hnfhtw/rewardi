/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : LedDriver.h
 * Purpose    : Driver for to switch the electromagnetic lock of the Rewardi Box
 ********************************************************************************************/

#ifndef COMPONENTS_LOCKDRIVER_H_
#define COMPONENTS_LOCKDRIVER_H_

#include <FreeRTOS.h>
#include <driver/gpio.h>
#include <freertos/timers.h>
#include "Box.h"

class Box;

class LockDriver {
public:
    LockDriver(gpio_num_t lockPin, Box* pBox);
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
    Box*            m_pBox;

};

#endif /* COMPONENTS_LOCKDRIVER_H_ */
