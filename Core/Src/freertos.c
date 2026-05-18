/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "stdio.h"
#include "fatfs_storage.h"
#include "stdlib.h"
#include "fatfs.h"
#include <string.h>
#include "dac.h"
#include "tim.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TAILLE_BUFFER 2048

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern uint8_t *uwInternelBuffer; //Buffer pour la mémoire SDRAM
extern char *pDirectoryFiles[];
extern uint8_t ubNumberOfFiles;
extern uint32_t uwBmplen;
extern uint8_t buffer[2 * TAILLE_BUFFER];
extern uint8_t bufflag;
extern uint8_t *Imgstock;


/* USER CODE END Variables */
osThreadId image_displayyHandle;
osMessageQId QueueTS2PICHandle;
osMessageQId QueueActivationHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void image_disp(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
__weak void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
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

  /* Create the queue(s) */
  /* definition and creation of QueueTS2PIC */
  osMessageQDef(QueueTS2PIC, 2, uint16_t);
  QueueTS2PICHandle = osMessageCreate(osMessageQ(QueueTS2PIC), NULL);

  /* definition and creation of QueueActivation */
  osMessageQDef(QueueActivation, 2, uint16_t);
  QueueActivationHandle = osMessageCreate(osMessageQ(QueueActivation), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of image_displayy */
  osThreadDef(image_displayy, image_disp, osPriorityAboveNormal, 0, 1024);
  image_displayyHandle = osThreadCreate(osThread(image_displayy), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_image_disp */
/**
  * @brief  Function implementing the image_display thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_image_disp */
void image_disp(void const * argument)
{
  /* USER CODE BEGIN image_disp */
	uint32_t counter = 0, transparency = 0;
	uint8_t str[30];
	FIL F1;
	FATFS SDFatFs;
	FRESULT res; /* FatFs function common result code */
	uint32_t byteswritten, bytesread; /* File write/read counts */
	uwInternelBuffer = (uint8_t*)  0xC0260000 ; // 0x90000000  NOR : 0x60000000
	uint32_t Size=261258;
	char text[60]={};
	/*##-1- Link the SD Card disk I/O driver ###################################*/
	if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {
		/*##-2- Initialize the Directory Files pointers (heap) ###################*/
		for (counter = 0; counter < MAX_BMP_FILES; counter++) {
			pDirectoryFiles[counter] = malloc(MAX_BMP_FILE_NAME);
		}

		/* Get the BMP file names on root directory */
		ubNumberOfFiles = Storage_GetDirectoryBitmapFiles("/Frame",
				pDirectoryFiles);

	} else {
		/* FatFs Initialization Error */
		Error_Handler();
	}

	/* Register the file system object to the FatFs module */
	if (f_mount(&SDFatFs, (TCHAR const*) SDPath, 0) == FR_OK) {

	}

  	counter = 0;
  	while ((counter) < ubNumberOfFiles) {
  		/* Format the string */
  		sprintf((char*) str, "Frame/%-11.11s", pDirectoryFiles[counter]);
  		if (Storage_CheckBitmapFile((const char*) str, &uwBmplen) == 0) {

  			/* Set LCD foreground Layer */
  			BSP_LCD_SelectLayer(1);

  			/* Open a file and copy its content to an internal buffer */
  			Storage_OpenReadFile(uwInternelBuffer, (const char*) str);

  			/* Write bmp file on LCD frame buffer */
  			HAL_GPIO_TogglePin(LED11_GPIO_Port,LED11_Pin);
  			BSP_LCD_DrawBitmap(0, 0, uwInternelBuffer);
  			HAL_GPIO_TogglePin(LED11_GPIO_Port,LED11_Pin);
  			sprintf(text," %2u / %2u ",counter+1, ubNumberOfFiles);
  			BSP_LCD_DisplayStringAt(15,220,(uint16_t*) text, LEFT_MODE);
  			/* Jump to the next image */
  			uwInternelBuffer+=Size;
  			counter++;

  		}
  	}

  /* Infinite loop */
  for(;;) {
	  counter = 0;
	  uwInternelBuffer =  0xC0260000 ; // 0x90000000  0x60000000
	  while ((counter) < ubNumberOfFiles) {
		  /* Set LCD foreground Layer */
			BSP_LCD_SelectLayer(1);

			BSP_LCD_DrawBitmap(0, 0, uwInternelBuffer);
			uwInternelBuffer+=Size;
			HAL_Delay(2);

			counter++;
	  }

  }
  /* USER CODE END image_disp */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

