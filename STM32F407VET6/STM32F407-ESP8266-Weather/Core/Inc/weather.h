#ifndef __WEATHER_H
#define __WEATHER_H

enum Weather{
    Sunny,
    Clear,
    Fair_Day,
    Fair_Night,
    Cloudy,
    Partly_Cloudy_Day,
    Partly_Cloudy_Night,
    Mostly_Cloudy_Day,
    Mostly_Cloudy_Night
};

/* 未来天气结构体 */
typedef struct future_weather_t{
    char city_name[32];             //城市名
    char date[12];                  //日期
    char weather_day[32];           //白天天气
    char weather_night[32];         //夜晚天气
    char high_temp[8];              //最高温度
    char low_temp[8];               //最低温度
}future_weather_t;

/* 实况天气结构体 */
typedef struct now_weather_t{
    char city[32];                  //城市名
    char date[12];                  //日期
    char weather[32];               //天气
    char temp[8];                      //温度
    char code[8];                      //天气代码
}now_weather_t;


/* 日期结构体 */
typedef struct date_t{
    char week[12];      //星期
    char month[12];     //月
    int day;            //日
    int year;           //年
    int hour;           //小时
    int minute;         //分钟
    int second;         //秒
}date_t;

now_weather_t parse_realtime_weather_data(const char *json_str); //解析实况天气
future_weather_t parse_future_weather_data(const char *json_str);   //解析未来天气
void parse_field(const char* str, const char* key, char* output, unsigned short max_len);
unsigned char parse_date(const char *date_str, date_t *dt);    //解析日期用于更新本地RTC
unsigned char intToBCD(int num);    //整数转BCD码
unsigned char getMonthNumberCaseInsensitive(const char* monthStr);  //根据月份字符串返回对应的BCD码
unsigned char getWeekNumberCaseInsensitive(const char* weekStr);    //根据星期几返回对应的整数，返回1~7

#endif
