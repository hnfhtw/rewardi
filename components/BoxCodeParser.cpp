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

#define BOXCODEPARSER_MINIMUMPRESS_DURATION_MS      100   // filter out very short presses
#define BOXCODEPARSER_SHORTPRESS_DURATION_MS        600
#define BOXCODEPARSER_LONGPRESS_DURATION_MS        1200
#define BOXCODEPARSER_SHORTPAUSE_DURATION_MS       1000   // short pause between two button presses
#define BOXCODEPARSER_LONGPAUSE_DURATION_MS        2500   // long pause between two commands
#define BOXCODEPARSER_STARTSTOPRECORD_DURATION_MS 10000   // 10s press starts (and stops) recording of a new 5-digit box code for current user
#define BOXCODEPARSER_WIFIRESET_DURATION_MS       30000   // press button 30s to reset wifi AP data and restart ESP32

static const char* LOG_TAG = "BoxCodeParser";

/**
 * @brief Create a BoxCodeParser instance.
 */
BoxCodeParser::BoxCodeParser(gpio_num_t buttonPin, Box* pBox){
    m_pButtonDriver = new ButtonDriver(buttonPin);
    m_pBox = pBox;
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
 * @brief xxx
 */
Box* BoxCodeParser::getBox(){
    return m_pBox;
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
        bool symbolTimerRunning = false;
        bool pauseTimerRunning = false;
        bool enablePauseTimer = false;
        uint32_t tick = 0;
        uint32_t symbolStartTick = 0;
        uint32_t symbolEndTick = 0;
        uint32_t symbolDeltaTick = 0;
        uint32_t pauseStartTick = 0;
        bool isSymbolParsed = false;

        uint8_t parsedCommand = 0;
        uint8_t parsedCommandIndex = 0;

        bool recordNewBoxCode = false;

        while (true) {   // Loop forever.
            isButtonPressed = !(m_pBoxCodeParser->getButtonDriver()->getLevel());

            // evaluate button press
            if(isButtonPressed && symbolTimerRunning == false){
                isSymbolParsed = false;
                symbolTimerRunning = true;
                pauseTimerRunning = false;
                enablePauseTimer = false;
                symbolStartTick = tick;
                parsedCommand |= (1 << 7);  // indicate that new command started
            }
            else if(isButtonPressed && symbolTimerRunning == true){
                m_pBoxCodeParser->getBox()->stayAwake();        // HN-CHECK Reset 60s stay awake timer to avoid going to sleep mode while user works with box
            }
            else if(symbolTimerRunning == true){
                symbolEndTick = tick;
                symbolDeltaTick = symbolEndTick - symbolStartTick;
                symbolTimerRunning = false;
                enablePauseTimer = true;

                if( (symbolDeltaTick > BOXCODEPARSER_MINIMUMPRESS_DURATION_MS) && (symbolDeltaTick <= BOXCODEPARSER_SHORTPRESS_DURATION_MS) ){    // add a 1 to the parsed command for each short press
                    ESP_LOGD(LOG_TAG, "Short Button Press, deltaTick = %d", symbolDeltaTick);
                    parsedCommand |= (1 << parsedCommandIndex);
                    if(parsedCommandIndex < 7){
                        ++parsedCommandIndex;
                    }
                }
                else if(symbolDeltaTick > BOXCODEPARSER_WIFIRESET_DURATION_MS){ // a button press for 30s will reset wifi AP data and restart ESP32
                    ESP_LOGD(LOG_TAG, "Wifi AP data reset button press detected");
                    // HN-CHECK Todo: Add reset here!!
                }
                else if(symbolDeltaTick > BOXCODEPARSER_STARTSTOPRECORD_DURATION_MS){   // a button press for 10s will start / stop box code recording for current user
                    if(recordNewBoxCode == true){
                        if ((parsedCommandIndex == 5) && (parsedCommand <= 159) && (parsedCommand >= 128) ){
                            uint8_t newBoxCode = parsedCommand & ~(1<<7);
                            ESP_LOGD(LOG_TAG, "Recorded 5 digit box code = %d", newBoxCode);
                            // inform box about new box code for current user
                            m_pBoxCodeParser->getBox()->setBoxCode(newBoxCode);
                            parsedCommandIndex = 0;
                            parsedCommand = 0;
                            pauseTimerRunning = false;
                            enablePauseTimer = false;
                        }

                        recordNewBoxCode = false;
                    }
                    else{
                        recordNewBoxCode = true;
                    }
                }
                else if(symbolDeltaTick > BOXCODEPARSER_LONGPRESS_DURATION_MS){     // add a 0 to the parsed command for each long press
                    ESP_LOGD(LOG_TAG, "Long Button Press, deltaTick = %d", symbolDeltaTick);
                    if(parsedCommandIndex < 7){
                        ++parsedCommandIndex;
                    }
                }
            }
            else if(enablePauseTimer == true){
                if(pauseTimerRunning == false){
                    pauseTimerRunning = true;
                    pauseStartTick = tick;
                }
                else if(pauseTimerRunning == true){
                    if( ((tick - pauseStartTick) > BOXCODEPARSER_SHORTPAUSE_DURATION_MS) && !isSymbolParsed){     // short pause between symbols
                        isSymbolParsed = true;
                    }
                    else if( (tick - pauseStartTick) > BOXCODEPARSER_LONGPAUSE_DURATION_MS){    // long pause between commands
                        ESP_LOGD(LOG_TAG, "Parsed command = %d, command lenght = %d", parsedCommand, parsedCommandIndex);
                        // Evaluate parsedCommand here!!
                        if( (parsedCommandIndex == 5) && (parsedCommand <= 159) && (parsedCommand >= 128) ){   // a 5 digit box code is detected
                            // HN-CHECK Todo -> inform box that a 5 digit box code was entered
                            uint8_t boxCode = parsedCommand & ~(1<<7);
                            m_pBoxCodeParser->getBox()->requestBoxOpen(boxCode);
                            ESP_LOGD(LOG_TAG, "5 digit box code = %d", boxCode);
                        }
                        else if( (parsedCommandIndex == 2) && (parsedCommand == 131) ){  // two short presses detected -> open box if it is not locked
                            if(m_pBoxCodeParser->getBox()->getIsLocked() == false){
                                m_pBoxCodeParser->getBox()->open();
                            }
                        }
                        parsedCommandIndex = 0;
                        parsedCommand = 0;
                        pauseTimerRunning = false;
                        enablePauseTimer = false;
                    }
                }
            }

            tick += 50;
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
