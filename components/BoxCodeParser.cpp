/*
 * BoxCodeParser.cpp
 *
 *  Created on: 29.11.2018
 *      Author: HN
 */


#include <string>
#include <esp_log.h>
#include "BoxCodeParser.h"
#include "Task.h"

#define BOXCODEPARSER_SHORTPRESS_DURATION_50MS  12
#define BOXCODEPARSER_LONGPRESS_DURATION_50MS   24
#define BOXCODEPARSER_PAUSE_DURATION_50MS       20

static const char* LOG_TAG = "BoxCodeParser";

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


/**
 * @brief xxx
 */
ButtonDriver* BoxCodeParser::getButtonDriver(){
    return m_pButtonDriver;
}


/**
 * @brief BoxCodeParserTask
 */
class BoxCodeParserTask: public Task {
public:
    BoxCodeParserTask(std::string name): Task(name, 16 * 1024) {
        m_pBoxCodeParser = nullptr;
    };

private:
    BoxCodeParser* m_pBoxCodeParser; // Reference to the BoxCodeParser

    /**
     * @brief xxx
     */
    void run(void* data) {
        m_pBoxCodeParser = (BoxCodeParser*) data;   // The passed in data is an instance of an BoxCodeParser.
        bool isButtonPressed = false;
        bool signTimerRunning = false;
        bool pauseTimerRunning = false;
        bool enablePauseTimer = false;
        uint32_t tick = 0;
        uint32_t signStartTick = 0;
        uint32_t signEndTick = 0;
        uint32_t signDeltaTick = 0;
        uint32_t pauseStartTick = 0;

        while (true) {   // Loop forever.
            isButtonPressed = !(m_pBoxCodeParser->getButtonDriver()->getLevel());

            // evaluate button press
            if(isButtonPressed && signTimerRunning == false){
                signTimerRunning = true;
                pauseTimerRunning = false;
                enablePauseTimer = false;
                signStartTick = tick;
            }
            else if(isButtonPressed && signTimerRunning == true){
            }
            else if(signTimerRunning == true){
                signEndTick = tick;
                signDeltaTick = signEndTick - signStartTick;
                signTimerRunning = false;
                enablePauseTimer = true;

                if(signDeltaTick <= BOXCODEPARSER_SHORTPRESS_DURATION_50MS){
                    ESP_LOGD(LOG_TAG, "Short Button Press, deltaTick = %d", signDeltaTick);
                    //myRunner = myRunner->nextDit;       // move on decode tree to the left (DIT)
                }
                else if((signDeltaTick > BOXCODEPARSER_LONGPRESS_DURATION_50MS)){
                    ESP_LOGD(LOG_TAG, "Long Button Press, deltaTick = %d", signDeltaTick);
                    //myRunner = myRunner->nextDah;       // move on decode tree to the right (DAH)
                }
            }
            else if(enablePauseTimer == true){
                if(pauseTimerRunning == false){
                    pauseTimerRunning = true;
                    pauseStartTick = tick;
                }
                else if(pauseTimerRunning == true){
                    if((tick - pauseStartTick) > BOXCODEPARSER_PAUSE_DURATION_50MS){
                        pauseTimerRunning = false;
                        enablePauseTimer = false;

                        /*char ch;
                        if(myRunner != NULL){
                            ch = myRunner->ch;          // get char of current node
                        }
                        if(ch != 0){
                            printf("%c ",ch);
                        }
                        else{
                            printf("? ");
                        }
                        myRunner = myHead;*/
                    }
                }

            }

            ++tick;
            vTaskDelay(pdMS_TO_TICKS(50));  // sleep 50ms
        } // while
    } // run
}; // BoxCodeParserTask

/**
 * @brief xxx
 */
void BoxCodeParser::start(){
    BoxCodeParserTask* pBoxCodeParserTask = new BoxCodeParserTask("BoxCodeParserTask");
    pBoxCodeParserTask->start(this);
}
