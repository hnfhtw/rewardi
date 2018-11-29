/*
 * BoxCodeParser.h
 *
 *  Created on: 29.11.2018
 *      Author: HN
 */

#ifndef COMPONENTS_BOXCODEPARSER_H_
#define COMPONENTS_BOXCODEPARSER_H_

#include "ButtonDriver.h"

class BoxCodeParser {
public:
    BoxCodeParser(gpio_num_t buttonPin);
    void            init();
private:
    ButtonDriver*   m_pButtonDriver;
};



#endif /* COMPONENTS_BOXCODEPARSER_H_ */
