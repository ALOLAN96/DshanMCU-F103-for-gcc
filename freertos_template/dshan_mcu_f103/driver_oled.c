// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com> 
 * Discourse:  https://forums.100ask.net
 */

 
/*  Copyright (C) 2008-2023 ���ڰ������Ƽ����޹�˾
 *  All rights reserved
 *
 *
 * ��������: ��������д���ĵ�������ѧԱѧϰʹ�ã�����ת��������(�뱣��������Ϣ)����ֹ������ҵ��;��
 * ��������: ��������д�ĳ��򣬿���������ҵ��;�������������е��κκ����
 * 
 * 
 * ��������ѭGPL V3Э�飬ʹ������ѭЭ�����
 * ���������õĿ����壺	DShanMCU-F103
 * ������Ƕ��ʽѧϰƽ̨��https://www.100ask.net
 * ��������������������	https://forums.100ask.net
 * �������ٷ�Bվ��				https://space.bilibili.com/275908810
 * �������ٷ��Ա���			https://100ask.taobao.com
 * ��ϵ����(E-mail)��	  weidongshan@qq.com
 *
 * ��Ȩ���У�����ؾ���
 *  
 * �޸���ʷ     �汾��           ����        �޸�����
 *-----------------------------------------------------
 * 2023.08.04      v01         ���ʿƼ�      �����ļ�
 *-----------------------------------------------------
 */


#include "driver_oled.h"
#include "ascii_font.c"//�˴�������ԭ���÷����ƣ���Ҫ�����ļ����˵�

#include "stm32f1xx_hal.h"

/*
    ȫ����������Ϩ��
*/
#define ENTIRE_DISP_ON()       OLED_WriteCmd(0xA5) 
#define ENTIRE_DISP_OFF()      OLED_WriteCmd(0xA4) 
/*
    ������ʾ����������ʾ
*/
#define DISP_NORMAL()          OLED_WriteCmd(0xA6)  
#define DISP_INVERSE()         OLED_WriteCmd(0xA7)
/*
    ����ʾ���߹ر���ʾ
*/
#define DISP_ON()              OLED_WriteCmd(0xAF) 
#define DISP_OFF()             OLED_WriteCmd(0xAE) 

/* 2. ��������ܺ��� */
typedef enum
{
    H_RIGHT     = 0x26,
    H_LEFT      = 0x27,
}H_SCROLL_DIR;  // ˮƽ��������


typedef enum
{
    HV_RIGHT    = 0x29,
    HV_LEFT     = 0x2A,
}HV_SCROLL_DIR;     // ˮƽ�ʹ�ֱ�����ķ���


/* 
    ��ʼ����ֹͣ����
*/
#define SCROLL_ON()             OLED_WriteCmd(0x2F)
#define SCROLL_OFF()            OLED_WriteCmd(0x2E)


/* 3. ��ַ���ù��ܺ��� */
typedef enum
{
    H_ADDR_MODE     = 0,    // ˮƽ��ַģʽ
    V_ADDR_MODE     = 1,    // ��ֱ��ַģʽ
    PAGE_ADDR_MODE  = 2,    // ҳ��ַģʽ
}MEM_MODE;  // �ڴ��ַģʽ


/*
    �е�ַ��ת���߲���ת
*/
#define OLED_SEG_REMAP()        OLED_WriteCmd(0xA1)
#define OLED_SEG_NOREMAP()      OLED_WriteCmd(0xA0)


/*
    COM����ɨ�跽������ɨ�������ɨ��
*/
#define OLED_SCAN_NORMAL()      OLED_WriteCmd(0xC0)
#define OLED_SCAN_REMAP()       OLED_WriteCmd(0xC8)


typedef enum
{
    COM_PIN_SEQ     = 0,
    COM_PIN_ALT     = 1,
}COM_PINS_MODE; // COM��������
typedef enum
{
    COM_NOREMAP     = 0,
    COM_REMAP       = 1,
}COM_REMAP_STATE;   // COM���ŷ�ת

    
typedef enum
{
    LEVEL_0     = 0x00,
    LEVEL_1     = 0x20,
    LEVEL_2     = 0x30,
}VCOMH_LEVEL;   // ��ѹ�ȼ�

/* 6. �����ײ���ܺ��� */
typedef enum
{
    PUMP_DISABLE    = 0,
    PUMP_ENABLE     = 1,
}CHARGE_PUMP_STATE; // �򿪻��߹رյ�ɱ�


#define OELD_I2C_ADDR 0x78
#define OLED_TIMEOUT  500
extern I2C_HandleTypeDef hi2c1;
static I2C_HandleTypeDef *g_pHI2COLED = &hi2c1;

/*
 *  ��������OLED_WriteCmd
 *  ����������I2C���������OLED
 *  ���������cmd-���͸�OLED������
 *  �����������
 *  ����ֵ��0-�ɹ�, ����ֵʧ��
 */
static int OLED_WriteCmd(uint8_t cmd)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = 0;
    tmpbuf[1] = cmd;
    
    return HAL_I2C_Master_Transmit(g_pHI2COLED, OELD_I2C_ADDR, tmpbuf, 2, OLED_TIMEOUT);
}

/*
 *  ��������OLED_WriteData
 *  ����������I2C���������OLED
 *  ���������data-���͸�OLED��д��GDRAM������
 *  �����������
 *  ����ֵ��0-�ɹ�, ����ֵʧ��
 */
static int OLED_WriteData(uint8_t data)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = 0x40;
    tmpbuf[1] = data;
    
    return HAL_I2C_Master_Transmit(g_pHI2COLED, OELD_I2C_ADDR, tmpbuf, 2, OLED_TIMEOUT);
}

/*
 *  ��������OLED_WriteNBytes
 *  ������������������N��д���Դ������
 *  ���������buf         - ����buffer
 *            length - ���ݸ���
 *  �����������
 *  ����ֵ��0-�ɹ�, ����ֵʧ��
 */
static int OLED_WriteNBytes(uint8_t *buf, uint16_t length)
{
    return HAL_I2C_Mem_Write(g_pHI2COLED, OELD_I2C_ADDR, 0x40, 1, buf, length, OLED_TIMEOUT);
}

/************** 1. ��������ܺ��� **************/
/*
 *  ��������OLED_SetContrastValue
 *  ��������������OLED�ĶԱȶȣ���������0x81--->���ͶԱȶ�ֵ
 *  ���������value --> �Աȶ�ֵ����Χ0~255
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetContrastValue(uint8_t value)
{
    OLED_WriteCmd(0x81);
    OLED_WriteCmd(value);
}

/************** 2. ��������ܺ��� **************/
/*
 *  ��������OLED_H_Scroll
 *  ������������ָ��ҳ�����س������õķ������õ�Ƶ��ˮƽ����
 *  ���������dir--->�������򣺳�����߳���
 *            start--->��ʼҳ
 *            fr_time--->�������ʱ�䣬ÿ����ô��֡ˮƽƫ��һ�й���
 *            end--->����ҳ
 *  �����������
 *  ����ֵ����
*/
static void OLED_H_Scroll(H_SCROLL_DIR dir, uint8_t start, uint8_t fr_time, uint8_t end)
{
    if((dir != H_RIGHT) && (dir != H_LEFT))     return;
    if((start>0x07) || (fr_time>0x07) || (end>0x07))    return;
    OLED_WriteCmd(dir);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(start);
    OLED_WriteCmd(fr_time);
    OLED_WriteCmd(end);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xFF);
}

/*
 *  ��������OLED_HV_Scroll
 *  ������������ָ��ҳ�����س������õķ������õ�Ƶ��ˮƽ�����һὫ���ص��е�ַ����ƫ��offset��ô�����λ
 *  ���������dir--->�������򣺳�����߳���
 *            start--->��ʼҳ
 *            fr_time--->�������ʱ�䣬ÿ����ô��֡ˮƽƫ��һ�й���
 *            end--->����ҳ
 *            offset--->��ƫ�Ƶ�λ
 *  �����������
 *  ����ֵ����
*/
static void OLED_HV_Scroll(HV_SCROLL_DIR dir, uint8_t start, uint8_t fr_time, uint8_t end, uint8_t offset)
{
    if((dir != HV_RIGHT) && (dir != HV_LEFT))     return;
    if((start>0x07) || (fr_time>0x07) || (end>0x07) || (offset>0x3F))    return;
    OLED_WriteCmd(dir);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(start);
    OLED_WriteCmd(fr_time);
    OLED_WriteCmd(end);
    OLED_WriteCmd(offset);
}

/*
 *  ��������OLED_SetVScrollArea
 *  ��������������OLED���ش�ֱ����������
 *  ���������area-->��������
 *            row_num--->������������
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetVScrollArea(uint8_t area, uint8_t row_num)
{
    if((area>0x3F) || (row_num>0x7F))       return;
    OLED_WriteCmd(0xA3);
    OLED_WriteCmd(area);
    OLED_WriteCmd(row_num);
}

/************** 3. ��ַ���ù��ܺ��� **************/
static MEM_MODE mem_mode = PAGE_ADDR_MODE;  // ��̬�ֲ�����������OLED�ĵ�ַģʽ��

/*
 *  ��������OLED_SetColAddr_PAGE
 *  ��������������OLED��ҳ��ַģʽ�µ���ʾ��ʼ�е�ַ
 *  ���������addr-->��ʼ�е�ַ
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetColAddr_PAGE(uint8_t addr)
{
    if(mem_mode != PAGE_ADDR_MODE)  return;
    if(addr > 0x7F)   return;
    OLED_WriteCmd(0x00 + (addr & 0x0F));
    OLED_WriteCmd(0x10 + (addr>>4));
}

/*
 *  ��������OLED_SetMemAddrMode
 *  ��������������OLED�ĵ�ַģʽ
 *  ���������mode-->��ַģʽ��ҳ��ַģʽ��ˮƽ��ַģʽ����ֱ��ַģʽ
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetMemAddrMode(MEM_MODE mode)
{
    if((mode != H_ADDR_MODE) && (mode != V_ADDR_MODE) && (mode != PAGE_ADDR_MODE))      return;
    OLED_WriteCmd(0x20);
    OLED_WriteCmd(mode);
    mem_mode = mode;
}

/*
 *  ��������OLED_SetColAddr_HV
 *  ��������������OLED��ˮƽ��ַģʽ��ֱ��ַģʽ��������ʾ����ʼ�е�ַ�ͽ����е�ַ
 *  ���������start-->��ʼ�е�ַ
 *            end --->�����е�ַ
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetColAddr_HV(uint8_t start, uint8_t end)
{
    if(mem_mode == PAGE_ADDR_MODE)      return;
    if((start > 127) || (end > 127))    return;
    OLED_WriteCmd(0x21);
    OLED_WriteCmd(start);
    OLED_WriteCmd(end);
}

/*
 *  ��������OLED_SetPageAddr_HV
 *  ��������������OLED��ˮƽ��ַģʽ��ֱ��ַģʽ��������ʾ����ʼҳ��ַ�ͽ���ҳ��ַ
 *  ���������start-->��ʼҳ��ַ
 *            end --->����ҳ��ַ
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetPageAddr_HV(uint8_t start, uint8_t end)
{
    if(mem_mode == PAGE_ADDR_MODE)      return;
    if((start > 7) || (end > 7))        return; 
    OLED_WriteCmd(0x22);
    OLED_WriteCmd(start);
    OLED_WriteCmd(end);
}

/*
 *  ��������OLED_SetPageAddr_PAGE
 *  ��������������OLED��ҳ��ַģʽ�µ���ʾ��ʼҳ��ַ
 *  ���������addr-->��ʼҳ��ַ
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetPageAddr_PAGE(uint8_t addr)
{
    if(mem_mode != PAGE_ADDR_MODE)  return;
    if(addr > 7)   return;
    OLED_WriteCmd(0xB0 + addr);
}

/************** 4. Ӳ�����ù��ܺ��� **************/
/*
 *  ��������OLED_SetDispStartLine
 *  ��������������OLED�ӵ�line�п�ʼ��ʾ������Ĭ�ϵ�0��ƫ����line��һ��
 *  ���������line-->��ʾ����ʼ��
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetDispStartLine(uint8_t line)
{
    if(line > 63)       return;
    OLED_WriteCmd(0x40 + line);
}

/*
 *  ��������OLED_SetMuxRatio
 *  ��������������OLED������
 *  ���������ratio-->������ֵ
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetMuxRatio(uint8_t ratio)
{
    if((ratio < 15) || (ratio > 63))      return;
    OLED_WriteCmd(0xA8);
    OLED_WriteCmd(ratio);
}

/*
 *  ��������OLED_SetDispOffset
 *  ��������������OLED��COM����ƫ��ֵ
 *  ���������offset-->COMƫ��ֵ
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetDispOffset(uint8_t offset)
{
    if(offset > 63)     return;
    OLED_WriteCmd(0xD3);
    OLED_WriteCmd(offset);
}

/*
 *  ��������OLED_SetComConfig
 *  ��������������OLED��COM��������
 *  ���������mode-->COM����ģʽ�������Ļ��ǿ�ѡ���
 *            state-->COM���ŷ�ת���
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetComConfig(COM_PINS_MODE mode, COM_REMAP_STATE state)
{
    if((mode != COM_PIN_SEQ) && (mode != COM_PIN_ALT))      return;
    if((state != COM_NOREMAP) && (state != COM_REMAP))      return;
    
    OLED_WriteCmd(0xDA);
    OLED_WriteCmd(0x02 + (mode << 4) + (state << 5));
}

/************** 5. ʱ�����ù��ܺ��� **************/
/*
 *  ��������OLED_SetDCLK_Freq
 *  ��������������OLED��ɨ�����ں;���Ƶ��
 *  ���������div-->ʱ�ӷ�Ƶϵ��
 *            freq-->����Ƶ��
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetDCLK_Freq(uint8_t div, uint8_t freq)
{
    if((div>0x0F) || (freq>0x0F))       return;
    OLED_WriteCmd(0xD5);
    OLED_WriteCmd(div + (freq<<4));
}

/*
 *  ��������OLED_SetPreChargePeriod
 *  ��������������OLED��Ԥ�������
 *  ���������phase1-->Ԥ���׶�1ʱ��
 *            phase2-->Ԥ���׶�2ʱ��
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetPreChargePeriod(uint8_t phase1, uint8_t phase2)
{
    if((phase1>0x0F) || (phase2>0x0F))       return;
    OLED_WriteCmd(0xD9);
    OLED_WriteCmd(phase1 + (phase2<<4));    
}

/*
 *  ��������OLED_SetVcomhLevel
 *  ��������������OLED�ĵ�ѹ�ȼ�
 *  ���������level-->��ѹ�ȼ�
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetVcomhLevel(VCOMH_LEVEL level)
{
    if((level != LEVEL_0) && (level != LEVEL_1) && (level != LEVEL_2))      return;
    OLED_WriteCmd(0xDB);
    OLED_WriteCmd(level);
}

/************** 6. �����ײ���ܺ��� **************/
/*
 *  ��������OLED_SetChargePump
 *  �����������򿪻�ر�OLED�ĵ�ɱ�
 *  ���������state-->��ɱô򿪻�ر�
 *  �����������
 *  ����ֵ����
*/
static void OLED_SetChargePump(CHARGE_PUMP_STATE state)
{
    if((state != PUMP_DISABLE) && (state != PUMP_ENABLE))   return;
    OLED_WriteCmd(0x8D);
    OLED_WriteCmd((state<<2) + 0x10);
}

/************** 7. ��ʼ������ **************/
/*
 *  ��������OLED_Init
 *  ������������ʼ��OLED
 *  �����������
 *  �����������
 *  ����ֵ����
 */
void OLED_Init(void)
{   
    /*
     * ǰ��: �Ѿ���ʼ����I2Cͨ��
     * ���������Ѿ�: 
     *    ʹ��MX_I2C1_Init��ʼ��I2Cͨ��
     *    ʹ��HAL_I2C_MspInit��ʼ��I2C����
     */
    
    OLED_SetMemAddrMode(PAGE_ADDR_MODE);    // 0. ���õ�ַģʽ
    OLED_SetMuxRatio(0x3F);                 // 1. ���ö�·������
    OLED_SetDispOffset(0x00);               // 2. ������ʾ��ƫ��ֵ
    OLED_SetDispStartLine(0x00);            // 3. ������ʼ��
    OLED_SEG_REMAP();                       // 4. �з�ת
    OLED_SCAN_REMAP();                      // 5. ����ɨ��
    OLED_SetComConfig(COM_PIN_SEQ, COM_NOREMAP);          // 6. COM ��������
    OLED_SetContrastValue(0x7F);            // 7. ���öԱȶ�
    ENTIRE_DISP_OFF();                      // 8. ȫ������/Ϩ��
    DISP_NORMAL();                          // 9. ��ʾģʽ
    OLED_SetDCLK_Freq(0x00, 0x08);          // 10. ���÷�Ƶϵ����Ƶ����ֵ
    OLED_SetChargePump(PUMP_ENABLE);        // 11. ʹ�ܵ����ײ
    
    OLED_SetComConfig(COM_PIN_ALT, COM_NOREMAP);
    
    DISP_ON();
}

/************** 8. �����������ܺ��� **************/
/*
 *  ��������OLED_SetPosition
 *  ��������������������ʾ����ʼҳ����ʼ�е�ַ
 *  ���������page-->ҳ��ַģʽ�µ���ʼҳ��ַ
 *            col-->ҳ��ַģʽ�µ���ʼ�е�ַ
 *  �����������
 *  ����ֵ����
*/
void OLED_SetPosition(uint8_t page, uint8_t col)
{
    OLED_SetPageAddr_PAGE(page);
    OLED_SetColAddr_PAGE(col);
}

/*
 *  ��������OLED_Clear
 *  ������������������
 *  �����������
 *  �����������
 *  ����ֵ����
*/
void OLED_Clear(void)
{
    uint8_t i = 0;
    uint8_t buf[128] = {0};
    
    for(i=0; i<8; i++)
    {
        OLED_SetPosition(i, 0);
        OLED_WriteNBytes(&buf[0], 128);
    }
}

/*
 *  ��������OLED_PutChar
 *  ������������ʾһ���ַ�
 *  ���������x --> x����(0~15)
 *            y --> y����(0~7)
 *            c -->   ��ʾ���ַ�
 *  �����������
 *  ����ֵ����
 */
void OLED_PutChar(uint8_t x, uint8_t y, char c)
{
    uint8_t page = y;
    uint8_t col  = x*8;
    
    if (y > 7 || x > 15)
        return;
    
    OLED_SetPosition(page, col);
    OLED_WriteNBytes((uint8_t*)&ascii_font[c][0], 8);
    
    OLED_SetPosition(page + 1, col);
    OLED_WriteNBytes((uint8_t*)&ascii_font[c][8], 8);
}

/*
 *  ��������OLED_PrintString
 *  ������������ʾһ���ַ���
 *  ���������x --> x����(0~15)
 *            y --> y����(0~7)
 *            str -->   ��ʾ���ַ���
 *  �����������
 *  ����ֵ����ӡ�˶��ٸ��ַ�
 */
int OLED_PrintString(uint8_t x, uint8_t y, const char *str)
{   
    int i = 0;
    while (str[i])
    {
        OLED_PutChar(x, y, str[i]);
        x++;
        if(x > 15)
        {
            x  = 0;
            y += 2;
        }
                
        i++;
    }
    return i;
}

/*
 *  ��������OLED_ClearLine
 *  �������������һ��
 *  ���������x - �����￪ʼ
 *            y - �������
 *  �����������
 *  ����ֵ����
 */
void OLED_ClearLine(uint8_t x, uint8_t y)
{
    for (;x < 16; x++)
    {
        OLED_PutChar(x, y, ' ');
    }
}

/*
 *  OLED_PrintHex
 *  ������������16������ʾ��ֵ
 *  ���������x - x����(0~15)
 *            y - y����(0~7)
 *            val -   ��ʾ������
 *            pre -   ����ʱ��ʾ"0x"ǰ׺
 *  �����������
 *  ����ֵ����ӡ�˶��ٸ��ַ�
 */
int OLED_PrintHex(uint8_t x, uint8_t y, uint32_t val, uint8_t pre)
{
    uint8_t hex_tab[]={'0','1','2','3','4','5','6','7',\
                             '8','9','a','b','c','d','e','f'};
	int i, j;
	char arr[11];

	/* ��ȡ���� */
    arr[0] = '0';
    arr[1] = 'x';
    arr[10] = '\0';

    j = 2;
	for (i = 7; i >= 0; i--)
	{
        arr[j] = hex_tab[(val >> (i*4)) & 0xf];
        if ((j != 2) || (arr[j] != '0'))
        {
            j++;
        }        
	}
    if (j == 2)
        j++;
    arr[j] = '\0';

    if (pre)
    {
        OLED_PrintString(x, y, arr);
        return j;
    }
    else
    {
        OLED_PrintString(x, y, arr+2);
        return j - 2;
    }
}


/*
 *  OLED_PrintSignedVal
 *  ������������10������ʾһ����ֵ
 *  ���������x --> x����(0~15)
 *            y --> y����(0~7)
 *            val -->   ��ʾ������
 *  �����������
 *  ����ֵ����ӡ�˶��ٸ��ַ�
*/
int OLED_PrintSignedVal(uint8_t x, uint8_t y, int32_t val)
{
    char str[16];
    char revert_str[16];
    int i = 0, j = 0;
    int mod;

    if (val < 0)
    {
        str[i++] = '-';
        val = 0 - val;
    }
    else if (val == 0)
    {
        str[i++] = '0';
    }

    while (val)
    {
        mod = val % 10;
        revert_str[j++] = mod + '0';
        val = val / 10;
    }

    while (j)
    {
        str[i++] = revert_str[j-1];
        j--;
    } 
    
    str[i] = '\0';
    OLED_PrintString(x, y, str);
    return i;
}


/**********************************************************************
 * �������ƣ� OLED_Test
 * ���������� OLED���Գ���
 * ��������� ��
 * ��������� ��
 *            ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���        �޸�����
 * -----------------------------------------------
 * 2023/08/03        V1.0     Τ��ɽ       ����
 ***********************************************************************/
void OLED_Test(void)
{
    OLED_Init();
	// ����
	OLED_Clear();
    
	while (1)
	{
		// ��(0, 0)��ӡ'A'
		OLED_PutChar(0, 0, 'A');
		// ��(1, 0)��ӡ'Y'
		OLED_PutChar(1, 0, 'Y');
		// �ڵ�0�е�2ҳ��ӡһ���ַ���"Hello World!"
		OLED_PrintString(0, 2, "Hello World!");
	}
}


