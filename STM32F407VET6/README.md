# STM32F4系列的工程文件

## STM32F407 Template

STM32F4系列标准库的工程模板



## STM32F407-LVGL-Template

STM32F4系列标准库和LVGL的模板，LVGL版本`8.3.10`，其中LVGL的界面设计使用`GUI GUIDER v1.9.1`，整体界面如下所示：

![1.png](./img/1.png)

整体目录结构：

```
├─CORE								//cotex m4所需要的文件
├─FWLIB								//标准库外设文件     
├─HARDWARE							//自定义的外设文件
├─LVGL								//LVGL库
│      LVGL_SRC						//LVGL移植的目录
│      UI							//GUI GUIDER生成UI文件存放的目录
├─OBJ								//Keil构建项目输出目录
├─SYSTEM							//包括串口、延时函数以及IO定义，直接复制的正点原子的案例
└─User								//主函数所在目录
│      .cmsis						//VS Code插件EIDE生成的文件夹，可删除
│      .edie						//VS Code插件EIDE生成的文件夹，可删除
│      .vscode						//VS Code插件EIDE生成的文件夹，可删除
```

## STM32F407-LVGL-FreeRTOS

将LVGL与FreeRTOS综合起来，实现了一个小demo。创建了3个任务：

- LVGL任务：每5ms执行一次刷新任务
- 温湿度读取任务：读取温湿度（模拟出来的，没有使用硬件），并刷新LCD。10S执行一次
- 按键任务：按下按键刷新标签控件，按下KEY0显示"Normal"，按下KEY1显示"Abnormal"。50ms执行一次

注：重写了**Delay延时函数**，使用定时器实现延时函数，避免与FreeRTOS争夺Systick时钟，造成程序无法运行的情况。

![2.png](./img/2.png)

FreeRTOS与LVGL加在一起比较耗资源，这个项目在EIDE下输出了以下信息：

```
Program Size: Code=182380 RO-data=74560 RW-data=788 ZI-data=81500  

Total RO  Size (Code + RO Data)               256940 ( 250.92kB)
Total RW  Size (RW Data + ZI Data)             82288 (  80.36kB)
Total ROM Size (Code + RO Data + RW Data)     257176 ( 251.15kB)

Total Memory Usage:

  RAM: [#############       ] 62.8%     80.4KB/128.0KB
  ROM: [##########          ] 49.1%     251.1KB/512.0KB
```

LVGL还可以进一步裁剪以减少SRAM和ROM

## STM32F407+ESP8266获取天气

基于HAL库实现，STM32F407VET6作为主控芯片，ESP8266-01S作为WIFI模块，并使用USB TO TTL连接电脑，查看解析结果。

连线如下表所示：

| STM32接线 | ESP8266 | USB TO TTL |
| --------- | ------- | ---------- |
| PA9       | RX      |            |
| PA10      | TX      |            |
| PB10      |         | RX         |
| PB11      |         | TX         |

考虑到ESP8266返回的JSON数据过大，如果使用串口会丢失数据，因此使用**DMA2来接收ESP8266发来的数据**，并通过**DMA1将解析后的数据发送**至串口助手中查看结果。

通过心知天气API获取到的JSON数据：

```json
{
  "results": [
    {
      "location": {
        "id": "WM6N2PM3WY2K",
        "name": "Chengdu",
        "country": "CN",
        "path": "Chengdu,Chengdu,Sichuan,China",
        "timezone": "Asia/Shanghai",
        "timezone_offset": "+08:00"
      },
      "daily": [
        {
          "date": "2025-10-21",
          "text_day": "Overcast",
          "code_day": "9",
          "text_night": "Overcast",
          "code_night": "9",
          "high": "17",
          "low": "12",
          "rainfall": "0.00",
          "precip": "0.00",
          "wind_direction": "NE",
          "wind_direction_degree": "45",
          "wind_speed": "8.4",
          "wind_scale": "2",
          "humidity": "88"
        }
      ],
      "last_update": "2025-10-21T08:00:00+08:00"
    }
  ]
}
```

可以cJSON来解析json数据，但是需要修改STM32启动文件的堆栈大小，本例程中使用字符串匹配的方式来进行了简单的解析。

测试结果：

![ESP8266_TEST.gif](./img/ESP8266_TEST.gif)

解析出的天气结果：

```shell
地区，日期，白天天气，晚上天气，最高温度，最低温度
Chengdu, 2025-10-21, Overcast, Overcast, 17, 12
```

