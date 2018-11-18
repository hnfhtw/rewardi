/*
 * StandbyControl.cpp
 *
 *  Created on: 18.11.2018
 *      Author: HN
 */

#include "StandbyControl.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include <esp_wifi.h>
#include <esp_log.h>

static const char* LOG_TAG = "StandbyControl";

/**
 * @brief xx
 */
StandbyControl::StandbyControl(gpio_num_t wakeupPin){
    m_wakeupPin = wakeupPin;    // GPIO_NUM_0
}

/**
 * @brief xx
 */
void StandbyControl::init(){
    rtc_gpio_init(m_wakeupPin);
    rtc_gpio_set_direction(m_wakeupPin, RTC_GPIO_MODE_INPUT_ONLY);    // set gpio pin 0 as input
    rtc_gpio_pullup_en(m_wakeupPin);
    rtc_gpio_pulldown_dis(m_wakeupPin);
    esp_sleep_enable_ext0_wakeup(m_wakeupPin, 0);        // wake-up with falling edge of gpio pin 0
}

/**
 * @brief xx
 */
void StandbyControl::enterSleepMode(){
    ESP_LOGD(LOG_TAG, "Enter Sleep Mode");
    esp_wifi_stop();
    esp_deep_sleep_start();     // enter deep sleep mode
    ESP_LOGD(LOG_TAG, "Exit Sleep Mode");
}
