/*
 * RgbLedControl.cpp
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#include "RgbLedControl.h"

/**
 * @brief xxx
 */
RgbLedControl::RgbLedControl(LedDriver* pLedRed, LedDriver* pLedGreen, LedDriver* pLedBlue){
    m_pLedRed = pLedRed;
    m_pLedGreen = pLedGreen;
    m_pLedBlue = pLedBlue;
}

/**
 * @brief xxx
 */
void RgbLedControl::setColor(RgbLedControl::Color color){
    switch(color){
        case BLACK: {
            m_pLedRed->switchOff();
            m_pLedGreen->switchOff();
            m_pLedBlue->switchOff();
            break;
        }
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


