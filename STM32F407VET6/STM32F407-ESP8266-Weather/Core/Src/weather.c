#include "weather.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static void toLowercase(char* str);

// 通用字符串字段解析函数
void parse_field(const char* str, const char* key, char* output, unsigned short max_len) {
    const char* ptr = strstr(str, key);
    if (ptr) {
        ptr += strlen(key);
        const char* end = strchr(ptr, '\"');
        if (end) {
            size_t len = end - ptr;
            if (len < max_len) {
                strncpy(output, ptr, len);
                output[len] = '\0';
            }
        }
    }
}

/**
 * @brief  解析未来3天的天气
 * @param  json_str: 要解析的json格式的字符串
 * @retval 无
 */
future_weather_t parse_future_weather_data(const char *json_str) {
    /* 初始化结构体 */
    future_weather_t wt = {
        .city_name = "null",
        .date = "null",
        .low_temp = "null",
        .high_temp = "null",
        .weather_day = "null",
        .weather_night = "null"
    };

    // 一次性找到daily数组开始位置
    const char* daily_start = strstr(json_str, "\"daily\":[");
    if (!daily_start) return wt;

    //解析城市名
    parse_field(json_str, "\"name\":\"", wt.city_name, sizeof(wt.city_name));
    
    //解析日期
    parse_field(daily_start, "\"date\":\"", wt.date, sizeof(wt.date));

    //解析白天天气
    parse_field(daily_start, "\"text_day\":\"", wt.weather_day, sizeof(wt.weather_day));

    //解析夜晚天气
    parse_field(daily_start, "\"text_night\":\"", wt.weather_night, sizeof(wt.weather_night));

    //解析最高温度
    parse_field(daily_start, "\"high\":\"", wt.high_temp, sizeof(wt.high_temp));

    //解析最低温度
    parse_field(daily_start, "\"low\":\"", wt.low_temp, sizeof(wt.low_temp));

    return wt;
}

/**
 * @brief 解析日期
 * 
 * @param date_str 包含日期的字符串
 * @return unsigned char 1：成功，0：失败
 */
unsigned char parse_date(const char *date_str, date_t *dt)
{
    /* 字符串为空，直接退出 */
    if(strlen(date_str) == 0)
        return 0;

    /* 获取包含日期的字符串的开始位置 */
    char *pos = strstr(date_str, "TIME:") + strlen("TIME:");
    char *end = strstr(date_str, "OK");

    unsigned short len = end - pos;

    char buffer[32];

    if(len > 0)
    {
        strncpy(buffer, pos, len);
        buffer[len] = '\0';

        if(sscanf(buffer, "%3s %3s %d %d:%d:%d %d", 
                dt->week, dt->month, &dt->day, 
                &dt->hour, &dt->minute, &dt->second, &dt->year) == 7)
        {
            // printf("Parse succeed!\n");
            return 1;
        }
        else
        {
            // printf("Parse failed!\n");
            return 0;
        }
    }

    return 0;
}

unsigned char intToBCD(int num) {
    if (num < 0 || num > 99) {
        // printf("错误数字必须在0-99范围内\n");
        return 0;
    }
    
    unsigned char tens = num / 10;      // 十位数：2
    unsigned char units = num % 10;     // 个位数：5
    unsigned char bcd = (tens << 4) | units;  // 组合成BCD码
    
    return bcd;
}

// 将字符串转换为小写
static void toLowercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

/**
 * @brief Get the Month Number Case Insensitive object
 * 
 * @param monthStr 月份缩写字符串，如"Oct"
 * @return unsigned char 
 */
unsigned char getMonthNumberCaseInsensitive(const char* monthStr) {
    const char* months[] = {"jan", "feb", "mar", "apr", "may", "jun",
                           "jul", "aug", "sep", "oct", "nov", "dec"};
    
    // 复制输入字符串并转换为小写
    char lowerMonth[4];
    strncpy(lowerMonth, monthStr, 3);
    lowerMonth[3] = '\0';
    toLowercase(lowerMonth);
    
    for (int i = 0; i < 12; i++) {
        if (strcmp(lowerMonth, months[i]) == 0) {
            return intToBCD(i+1);   //返回月份的BCD码，例如返回0x12表示12月份
        }
    }
    return 1;   //匹配失败则默认设置为一月
}

unsigned char getWeekNumberCaseInsensitive(const char* weekStr) {
    const char* weekdays[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
    
    // 复制输入字符串并转换为小写
    char lowerWeek[4];
    strncpy(lowerWeek, weekStr, 3);
    lowerWeek[3] = '\0';
    toLowercase(lowerWeek);
    
    for (int i = 0; i < 7; i++) {
        if (strcmp(lowerWeek, weekdays[i]) == 0) {
            return i + 1;   //返回星期几对应的值，1：表示星期一，7表示星期天
        }
    }
    return 1;   //匹配失败默认设置为星期一
}
