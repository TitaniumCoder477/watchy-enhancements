#include "Watchy_7_SEG.h"

#define DARKMODE true

const uint8_t BATTERY_SEGMENT_WIDTH = 7;
const uint8_t BATTERY_SEGMENT_HEIGHT = 11;
const uint8_t BATTERY_SEGMENT_SPACING = 9;
const uint8_t WEATHER_ICON_WIDTH = 48;
const uint8_t WEATHER_ICON_HEIGHT = 32;

RTC_DATA_ATTR int minutesCountdown = 0;

void Watchy7SEG::drawWatchFace(){
    display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
    display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    drawTime();
    drawDate();
    drawCountdown();
    drawWeather();
    drawBattery();
    display.drawBitmap(125, 6, WIFI_CONFIGURED ? wifi : wifioff, 26, 18, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    //if(BLE_CONFIGURED){
        //display.drawBitmap(100, 75, bluetooth, 13, 21, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    //}
}

void Watchy7SEG::drawTime(){
    display.setFont(&DSEG7_Classic_Bold_53);
    display.setTextWrap(false);
    //display.setCursor(5, 53+5);
    display.setCursor(5, 130);
    int displayHour;
    if(HOUR_12_24==12){
      displayHour = ((currentTime.Hour+11)%12)+1;
    } else {
      displayHour = currentTime.Hour;
    }
    if(displayHour < 10){
        display.print("0");
    }
    display.print(displayHour);
    display.print(":");
    if(currentTime.Minute < 10){
        display.print("0");
    }
    display.println(currentTime.Minute);
}

void Watchy7SEG::drawCountdown() {
    if (minutesCountdown > 0) {
      if (minutesCountdown == 1) {
        vibMotor(75, 12);
        delay(1000);
        vibMotor(75, 12);
        delay(1000);
        vibMotor(75, 12);
        delay(1000);
        vibMotor(75, 12);
        delay(1000);
        vibMotor(75, 12);
        delay(1000);
        vibMotor(75, 12);
        delay(1000);
        vibMotor(75, 12);
      }
      minutesCountdown--;
    }

    display.setFont(&DSEG7_Classic_Bold_25);
    display.setCursor(105, 160);
    if(minutesCountdown < 10) {
      display.print("0");
    }
    display.println(minutesCountdown);

    display.setFont(&Seven_Segment10pt7b);
    display.setCursor(150, 149);
    display.println("min");
}

void Watchy7SEG::drawDate(){
    display.setFont(&Seven_Segment10pt7b);

    int16_t  x1, y1;
    uint16_t w, h;

    String dayOfWeek = dayStr(currentTime.Wday);
    display.getTextBounds(dayOfWeek, 5, 85, &x1, &y1, &w, &h);
    if(currentTime.Wday == 4){
        w = w - 5;
    }
    display.setCursor(200 - w - 23, 70);
    display.println(dayOfWeek);

    String month = monthShortStr(currentTime.Month);
    display.getTextBounds(month, 60, 110, &x1, &y1, &w, &h);
    display.setCursor(200 - w - 49, 200 - h + 8);
    display.println(month);

    display.setFont(&DSEG7_Classic_Bold_25);
    display.setCursor(200 - w - 16, 200 - h + 9);
    if(currentTime.Day < 10) {
      display.print("0");
    }
    display.println(currentTime.Day);
}

void Watchy7SEG::drawSteps(){
    // reset step counter at midnight
    if (currentTime.Hour == 0 && currentTime.Minute == 0){
      sensor.resetStepCounter();
    }
    uint32_t stepCount = sensor.getCounter();
    display.drawBitmap(10, 165, steps, 19, 23, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    display.setCursor(35, 190);
    display.println(stepCount);
}

void Watchy7SEG::drawBattery(){
    display.drawBitmap(158, 5, battery, 37, 21, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    display.fillRect(163, 10, 27, BATTERY_SEGMENT_HEIGHT, DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);//clear battery segments
    int8_t batteryLevel = 0;
    float VBAT = getBatteryVoltage();
    if(VBAT > 4.1){
        batteryLevel = 3;
    }
    else if(VBAT > 3.95 && VBAT <= 4.1){
        batteryLevel = 2;
    }
    else if(VBAT > 3.80 && VBAT <= 3.95){
        batteryLevel = 1;
    }
    else if(VBAT <= 3.80){
        batteryLevel = 0;
    }

    for(int8_t batterySegments = 0; batterySegments < batteryLevel; batterySegments++){
        display.fillRect(163 + (batterySegments * BATTERY_SEGMENT_SPACING), 10, BATTERY_SEGMENT_WIDTH, BATTERY_SEGMENT_HEIGHT, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    }
}

void Watchy7SEG::drawWeather(){

    weatherData currentWeather = getWeatherData();

    int8_t temperature = currentWeather.temperature;
    int16_t weatherConditionCode = currentWeather.weatherConditionCode;

    display.setFont(&DSEG7_Classic_Regular_39);
    int16_t  x1, y1;
    uint16_t w, h;
    display.setFont(&DSEG7_Classic_Bold_25);
    display.getTextBounds(String(temperature), 0, 0, &x1, &y1, &w, &h);
    display.setCursor(5, 200 - 5 - h - y1);
    display.println(temperature);
    display.drawBitmap(5 + w + x1 + 5, 170, currentWeather.isMetric ? celsius : fahrenheit, 26, 20, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    const unsigned char* weatherIcon;

    if(WIFI_CONFIGURED){
      //https://openweathermap.org/weather-conditions
      if(weatherConditionCode > 801) {//Cloudy
        weatherIcon = cloudy;
      } else if(weatherConditionCode == 801) {//Few Clouds
        weatherIcon = cloudsun;
      } else if(weatherConditionCode == 800) {//Clear
        weatherIcon = sunny;
      } else if(weatherConditionCode >= 700) {//Atmosphere
        weatherIcon = atmosphere;
      } else if(weatherConditionCode >= 600) {//Snow
        weatherIcon = snow;
      } else if(weatherConditionCode >= 500) {//Rain
        weatherIcon = rain;
      } else if(weatherConditionCode >= 300) {//Drizzle
        weatherIcon = drizzle;
      } else if(weatherConditionCode >= 200) {//Thunderstorm
        weatherIcon = thunderstorm;
      } else 
        return;
    } else {
      weatherIcon = chip;
    }
    
    display.drawBitmap(5, 5, weatherIcon, WEATHER_ICON_WIDTH, WEATHER_ICON_HEIGHT, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}