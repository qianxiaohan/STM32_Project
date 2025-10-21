#include "weather.h"
#include <string.h>

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
