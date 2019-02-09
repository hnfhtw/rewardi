/********************************************************************************************
 * Project    : Rewardi
 * Created on : 18.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : RgbLedControl.cpp
 * Purpose    : Driver to control the RGB LED of a Rewardi Box
 ********************************************************************************************/

#include <map>
#include "RgbLedControl.h"

static std::map<void*, RgbLedControl*> rgbLedControlMap;

/**
 * @brief RgbLedControl constructor - needs the three pins where the red, green and blue LEDs are connected
 */
RgbLedControl::RgbLedControl(gpio_num_t ledRedPin, gpio_num_t ledGreenPin, gpio_num_t ledBluePin){
    m_pLedRed = new LedDriver(ledRedPin);       // create LedDriver object for red LED
    m_pLedGreen = new LedDriver(ledGreenPin);   // create LedDriver object for green LED
    m_pLedBlue = new LedDriver(ledBluePin);     // create LedDriver object for blue LED
    m_hBlinkTimer = nullptr;
    m_period = OFF;
    m_color = RgbLedControl::Color::WHITE;
    m_time = 0;
}

/**
 * @brief Destructor
 */
RgbLedControl::~RgbLedControl(){
    ::xTimerDelete(m_hBlinkTimer, portMAX_DELAY);
    rgbLedControlMap.erase(m_hBlinkTimer);
}

/**
 * @brief Initialize RgbLedControl -> init the three LedDrivers and create a timer that will be used to blink the LED
 *        Map the RgbLedControl object with this timer
 */
void RgbLedControl::init(){
    m_pLedRed->init();
    m_pLedGreen->init();
    m_pLedBlue->init();
    m_hBlinkTimer = xTimerCreate("ledBlinkTimer",  pdMS_TO_TICKS(500), pdTRUE, NULL, (TimerCallbackFunction_t) &RgbLedControl::timeout);
    rgbLedControlMap.insert(std::make_pair(m_hBlinkTimer, this));
}

/**
 * @brief Start the blink timer
 */
void RgbLedControl::start(){
    xTimerStart(m_hBlinkTimer, 0);   // start LED blink timer
}

/**
 * @brief Set RGB LED color
 */
void RgbLedControl::setColor(RgbLedControl::Color color){
    m_color = color;
}

/**
 * @brief Set RGB LED blinking period
 */
void RgbLedControl::setPeriod(RgbLedControl::Period period){
    m_period = period;
}

/**
 * @brief Get RGB LED color
 */
RgbLedControl::Color RgbLedControl::getColor(){
    return m_color;
}

/**
 * @brief Get RGB LED blinking period
 */
RgbLedControl::Period RgbLedControl::getPeriod(){
    return m_period;
}

/**
 * @brief Update the output pin values for the three LEDs
 */
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
 * @brief Called each 500ms by the blinking timer -> used to blink the LED if desired
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


