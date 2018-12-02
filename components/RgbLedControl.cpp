/*
 * RgbLedControl.cpp
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#include <map>
#include "RgbLedControl.h"


static std::map<void*, RgbLedControl*> rgbLedControlMap;

/**
 * @brief xxx
 */
RgbLedControl::RgbLedControl(gpio_num_t ledRedPin, gpio_num_t ledGreenPin, gpio_num_t ledBluePin){
    m_pLedRed = new LedDriver(ledRedPin);
    m_pLedGreen = new LedDriver(ledGreenPin);
    m_pLedBlue = new LedDriver(ledBluePin);
    m_hBlinkTimer = nullptr;
    m_period = OFF;
    m_color = RgbLedControl::Color::WHITE;
    m_time = 0;
}

/**
 * @brief xx
 */
RgbLedControl::~RgbLedControl(){
    ::xTimerDelete(m_hBlinkTimer, portMAX_DELAY);
    rgbLedControlMap.erase(m_hBlinkTimer);
}

/**
 * @brief xxx
 */
void RgbLedControl::init(){
    m_pLedRed->init();
    m_pLedGreen->init();
    m_pLedBlue->init();
    m_hBlinkTimer = xTimerCreate("ledBlinkTimer",  pdMS_TO_TICKS(500), pdTRUE, NULL, (TimerCallbackFunction_t) &RgbLedControl::timeout);
    rgbLedControlMap.insert(std::make_pair(m_hBlinkTimer, this));
}


/**
 * @brief xx
 */
void RgbLedControl::start(){
    xTimerStart(m_hBlinkTimer, 0);   // start LED blink timer
}


/**
 * @brief xxx
 */
void RgbLedControl::setColor(RgbLedControl::Color color){
    m_color = color;
}


/**
 * @brief xxx
 */
void RgbLedControl::setPeriod(RgbLedControl::Period period){
    m_period = period;
}


/**
 * @brief xxx
 */
RgbLedControl::Color RgbLedControl::getColor(){
    return m_color;
}


/**
 * @brief xxx
 */
RgbLedControl::Period RgbLedControl::getPeriod(){
    return m_period;
}



void RgbLedControl::updateOutputValues(bool isOutputOn){
    if(isOutputOn == false){
        m_pLedRed->switchOff();
        m_pLedGreen->switchOff();
        m_pLedBlue->switchOff();
        return;
    }

    switch(m_color){
            case WHITE: {
                m_pLedRed->switchOn();
                m_pLedGreen->switchOn();
                m_pLedBlue->switchOn();
                break;
            }
            case RED: {
                m_pLedRed->switchOn();
                m_pLedGreen->switchOff();
                m_pLedBlue->switchOff();
                break;
            }
            case GREEN: {
                m_pLedRed->switchOff();
                m_pLedGreen->switchOn();
                m_pLedBlue->switchOff();
                break;
            }
            case BLUE: {
                m_pLedRed->switchOff();
                m_pLedGreen->switchOff();
                m_pLedBlue->switchOn();
                break;
            }
            case YELLOW: {
                m_pLedRed->switchOn();
                m_pLedGreen->switchOn();
                m_pLedBlue->switchOff();
                break;
            }
            case CYAN: {
                m_pLedRed->switchOff();
                m_pLedGreen->switchOn();
                m_pLedBlue->switchOn();
                break;
            }
            case MAGENTA: {
                m_pLedRed->switchOn();
                m_pLedGreen->switchOff();
                m_pLedBlue->switchOn();
                break;
            }
            default: {
                m_pLedRed->switchOff();
                m_pLedGreen->switchOff();
                m_pLedBlue->switchOff();
                break;
            }
        }
}


/**
 * @brief xxx
 */
void RgbLedControl::timeout(TimerHandle_t xTimer){
    RgbLedControl* pRgbControl =rgbLedControlMap.at(xTimer);
    switch(pRgbControl->getPeriod()){
        case OFF: {
            pRgbControl->updateOutputValues(false);
            break;
        }
        case ON: {
            pRgbControl->m_time = 0;
            pRgbControl->updateOutputValues(true);
            break;
        }
        case MS_1000: {
            if(pRgbControl->m_time == 0){
                pRgbControl->updateOutputValues(true);
                pRgbControl->m_time = 1;
            }
            else if(pRgbControl->m_time == 1){
                pRgbControl->updateOutputValues(false);
                pRgbControl->m_time = 0;
            }
            else{
                pRgbControl->m_time = 0;
            }
            break;
        }
        case MS_2000: {
            if(pRgbControl->m_time < 2){
                pRgbControl->updateOutputValues(true);
                ++pRgbControl->m_time;
            }
            else if(pRgbControl->m_time < 4){
                pRgbControl->updateOutputValues(false);
                ++pRgbControl->m_time;
            }
            else{
                pRgbControl->m_time = 0;
            }
            break;
        }
        default: {
            break;
        }
    }
}


