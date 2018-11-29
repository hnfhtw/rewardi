/*
 * BoxCodeParser.cpp
 *
 *  Created on: 29.11.2018
 *      Author: HN
 */


#include "BoxCodeParser.h"

/**
 * @brief Create a BoxCodeParser instance.
 */
BoxCodeParser::BoxCodeParser(gpio_num_t buttonPin){
    m_pButtonDriver = new ButtonDriver(buttonPin);
}


/**
 * @brief xxx
 */
void BoxCodeParser::init(){
    m_pButtonDriver->init();
}

