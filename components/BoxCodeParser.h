/********************************************************************************************
 * Project    : Rewardi
 * Created on : 29.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : BoxCodeParser.h
 * Purpose    : Each Rewardi Box has a BoxCodeParser that parses user inputs on the push button
 ********************************************************************************************/

#ifndef COMPONENTS_BOXCODEPARSER_H_
#define COMPONENTS_BOXCODEPARSER_H_

#include "ButtonDriver.h"
#include "Box.h"

class Box;

class BoxCodeParser {
public:
    BoxCodeParser(gpio_num_t buttonPin, Box* pBox);
    void            init();
    void            start();
    ButtonDriver*   getButtonDriver();
    Box*            getBox();
private:
    ButtonDriver*   m_pButtonDriver;
    Box*            m_pBox;
};

#endif /* COMPONENTS_BOXCODEPARSER_H_ */
