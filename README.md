## STM32��������˵����

### ʹ�ù��ߣ�

#### MCU��STM32F103C8T6

#### CubeMX��V6.9.2

#### VSCode����װEIDE��� ���м����˱��롢���ء����Եȶ��⿪�����

#### ������빤������(GNU Arm Embedded Toolchain 10-2020-q4-major) 10.2.1 20201103 (release)

### ����ʹ�ò���˵����

#### ��������˵����http://t.csdnimg.cn/COQKi

#### EIDEʹ����̳��[Embedded IDE Forum (em-ide.com)](https://discuss.em-ide.com/)





## ��Ŀ�����ļ���˵��

### 01_freertos_template

һ��ʹ��CubeMX��ʼ��STM32F103C8T6��������ģ�幤��

### 02_freertos_create_the_first_multitask

ʹ��CubeMX����������

### 03_freertos_create_multitask

ʹ��xTaskCreate/xTaskCreateStatic�ֶ�����������

```C
BaseType_t xTaskCreate ( TaskFunction_t pxTaskCode, // ����ָ��, ������
                        const char * const pcName, // ���������
                        const configSTACK_DEPTH_TYPE usStackDepth, // ջ��С,��λΪword,10��ʾ40�ֽ�
                        void * const pvParameters, // ����������ʱ����Ĳ���
                        UBaseType_t uxPriority,    // ���ȼ�
                        TaskHandle_t * const pxCreatedTask ); // ������, �Ժ�ʹ�����������������
```

|   **����**    | **����**                                                     |
| :-----------: | :----------------------------------------------------------- |
|  pvTaskCode   | ����ָ�룬�����Ӧ�� C ����������Ӧ����Զ���˳����������˳�ʱ���� "vTaskDelete(NULL)"�� |
|    pcName     | ��������ƣ������ڵ���Ŀ�ģ�FreeRTOS �ڲ���ʹ�á�pcName �ĳ���Ϊ configMAX_TASK_NAME_LEN |
| usStackDepth  | ÿ���������Լ���ջ��usStackDepth ָ����ջ�Ĵ�С����λΪ word�����磬������� 100����ʾջ�Ĵ�СΪ 100 word���� 400 �ֽڡ����ֵΪ uint16_t �����ֵ��ȷ��ջ�Ĵ�С�������ף�ͨ���Ǹ��ݹ������趨����ȷ�İ취�ǲ鿴�������롣 |
| pvParameters  | ���� pvTaskCode ����ָ��ʱʹ�õĲ�����pvTaskCode(pvParameters)�� |
|  uxPriority   | ��������ȼ���ΧΪ 0~(configMAX_PRIORITIES �C 1)����ֵԽС�����ȼ�Խ�͡���������ֵ����xTaskCreate �Ὣ�����Ϊ (configMAX_PRIORITIES �C 1)�� |
| pxCreatedTask | ���ڱ��� xTaskCreate ����������������ľ����task handle��������Ժ���Ҫ�Ը�������в��������޸����ȼ�������Ҫʹ�ô˾�����������Ҫʹ�øþ�������Դ��� NULL�� |

```c
TaskHandle_t xTaskCreateStatic ( TaskFunction_t pxTaskCode,   // ����ָ��, ������
                                 const char * const pcName,   // ���������
                                 const uint32_t ulStackDepth, // ջ��С,��λΪword,10��ʾ40�ֽ�
                                 void * const pvParameters,   // ����������ʱ����Ĳ���
                                 UBaseType_t uxPriority,      // ���ȼ�
                                 StackType_t * const puxStackBuffer, // ��̬�����ջ������һ��buffer
                                 StaticTask_t * const pxTaskBuffer );// ��̬���������ṹ���ָ�룬�����������������
```

### 04_freertos_create_multitask_with_para

ʹ�ýṹ�崫�ݲ���

### 05_freertos_delete_task

ʹ��vTaskDeleteɾ������

```c
void vTaskDelete ( TaskHandle_t xTaskToDelete );
NULL��
```



### 06_freertos_task_priority



### 07_freertos_task_suspend



### 08_freertos_idle_task