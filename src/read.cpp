#include <Arduino.h>
#include "WString.h"
#include <iostream>
#include <stdint.h>
#include <string>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "SD_MMC.h"
#include "epdiy.h"

// sd卡mmc连接接口
#define clk 2
#define cmd 38
#define d0 1

#define UART_NUM UART_NUM_1  // 使用 UART1
#define TX_PIN 43            // ESP32-S3 的 TX 引脚
#define RX_PIN 44            // ESP32-S3 的 RX 引脚
#define BUFFER_SIZE 4096

void init_uart() {
  // 初始化 UART
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);   // 初始化 UART1
  Serial1.println("UART1 initialized successfully!");  // 使用主串口打印调试信息
}



#pragma pack(push, 1)  //用于强制禁止结构体4Bit对齐


typedef struct BMPHARD  // bmp文件头
{
  unsigned short bfType;  // 文件的类型，该值必需是0x4D42，也就是字符'BM'。

  unsigned long bfSize;  // 位图文件的大小，用字节为单位

  unsigned short bfReserved1;  // 保留，必须设置为0

  unsigned short bfReserved2;  // 保留，必须设置为0

  unsigned long bfOffBits;  // 位图数据距离文件开头偏移量，用字节为单位
} BMPHARD;

typedef struct BMPDATAHARD  // bmp位图信息头
{
  unsigned long biSize;  // BITMAPINFOHEADER结构所需要的字数

  unsigned long biWidth;  // 图像宽度，单位为像素

  unsigned long biHeight;  // 图像高度，单位为像素，负数，则说明图像是正向的

  unsigned short biPlanes;  // 为目标设备说明位面数，其值将总是被设为1

  unsigned short biBitCount;  // 一个像素占用的bit位，值位1、4、8、16、24、32

  unsigned long biCompression;  // 压缩类型

  unsigned long biSizeImage;  // 位图数据的大小，以字节为单位

  unsigned long biXPelsPerMeter;  // 水平分辨率，单位 像素/米

  unsigned long biYPelsPerMeter;  // 垂直分辨率，单位 像素/米

  unsigned long biClrUsed;  //

  unsigned long biClrImportant;  //
} BMPDATAHARD;
#pragma pack(pop)  //用于强制禁止结构体4Bit对齐



// 加载sd卡
bool load_SD() {
  SD_MMC.setPins(clk, cmd, d0); /* 改变默认引脚 */
  if (SD_MMC.begin("/sdcard", true)) {
    Serial1.println("SD卡挂载成功");
    return true;
  } else {
    Serial1.println("SD卡挂载失败");
    return false;
  }
}





int read(EpdRect image_area, uint8_t *framebuffer, int name) {
  File bmp;
  // "/badapple/"
  load_SD();
  String ss = "/badapple/" + String(name) + ".bmp";
  bmp = SD_MMC.open(ss);
  init_uart();
  // file=SD_MMC.open("/data.json");
  if (!bmp) {
    Serial1.println("文件打开失败。");
    return 0;
  }
  Serial1.println("文件打开成功。" + String(name));

  BMPHARD bmphard;
  BMPDATAHARD bmpdatagard;
  //读取png标头
  bmp.readBytes((char *)&bmphard, sizeof(BMPHARD));
  bmp.readBytes((char *)&bmpdatagard, sizeof(bmpdatagard));
  // bmpdatagard.biWidth = 2560;
  // bmpdatagard.biHeight = 1600;
  int off = ((bmpdatagard.biWidth / 2 + 3) / 4) * 4 - bmpdatagard.biWidth / 2;  //计算文件实际行长度下的偏移值
  // uint32_t value_index;
  // uint32_t temp;
  uint8_t image_data[2560 / 2];          //暂存行数据
  bmp.seek(bmphard.bfOffBits, SeekSet);  //跳转至数据段
  for (int i = bmpdatagard.biHeight - 1; i >= 0; i--) {
    bmp.readBytes((char *)&image_data, bmpdatagard.biWidth / 2);  //读取行数据
    if(i%2==0){
      for (int j=0;j<bmpdatagard.biWidth/2;j++){
        framebuffer[i*bmpdatagard.biWidth/2+j]=image_data[j];
      }
    }
    else{
      for (int j=0;j<bmpdatagard.biWidth/2;j++){
        framebuffer[i*bmpdatagard.biWidth/2+j]=(image_data[j] << 4) | (image_data[j] >> 4);
      }
    }


    // bmp.readBytes((char *) &data[(i)*(bmpdatagard.biWidth)/2],bmpdatagard.biWidth/2);//读取一行数据
    bmp.seek(off, SeekCur);  //跳过填充段
  }
  Serial1.printf("读取完成");
  return 0;
}