/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "driver_led.h"
#include "driver_lcd.h"
#include "driver_passive_buzzer.h"
#include "driver_ir_receiver.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for startTask */
osThreadId_t startTaskHandle;
const osThreadAttr_t startTask_attributes = {
    .name       = "startTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void MUSIC_Analysis(void);
void AppStartTask(void *argument);
void PlayMusic(void *params);
/* USER CODE END FunctionPrototypes */

void TaskBuzzer(void *argument);
void TaskLed(void *argument);
void TaskColorLed(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of startTask */
    startTaskHandle = osThreadNew(AppStartTask, NULL, &startTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_AppStartTask */
/**
 * @brief  Function implementing the startTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_AppStartTask */
void AppStartTask(void *argument)
{
    /* USER CODE BEGIN AppStartTask */
    /* Oled ��ʼ�� */
    LCD_Init();
    LCD_Clear();
    LCD_PrintString(0, 0, "Waiting Control");

    /* ������մ�������ʼ�� */
    IRReceiver_Init();

    uint8_t dev, data;
    int len;
    TaskHandle_t xBuzzerTaskHandle, xOledTaskhandle, xLedTaskhandle;
    xBuzzerTaskHandle = xOledTaskhandle = xLedTaskhandle = NULL;

    /* ���������� */
    xTaskCreate(TaskLed, "Oled Task", 128, NULL, osPriorityNormal, &xOledTaskhandle);

    /* Infinite loop */
    for (;;) {
        if (0u == IRReceiver_Read(&dev, &data)) {
            if (data == 0xa8u) {
                /* Play �������� */
                /* ���������� */
                if (xBuzzerTaskHandle == NULL) {
                    LCD_ClearLine(0, 0);
                    LCD_PrintString(0, 0, "Create to Music");
                    xTaskCreate((void (*)(void *))PlayMusic, "Buzzer Task", 128, NULL, osPriorityNormal, &xBuzzerTaskHandle);
                }
            } else if (data == 0xa2u) {
                if (xBuzzerTaskHandle != NULL) {
                    /* ɾ�������� */
                    LCD_ClearLine(0, 0);
                    LCD_PrintString(0, 0, "Delete Task");
                    vTaskDelete(xBuzzerTaskHandle);
                    PassiveBuzzer_Control(0); // ������Ȼɾ�������Ƿ������԰��趨�Ĺ�������Ҫ�ֶ�ֹͣ
                    xBuzzerTaskHandle = NULL;
                }
            }

            len = LCD_PrintString(0, 6, "Key name: ");
            LCD_PrintString(len, 6, IRReceiver_CodeToString(data));
        }
    }
    /* USER CODE END AppStartTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void TaskLed(void *argument)
{
    for (;;) {
        Led_Test();
    }
}
/* USER CODE END Application */
