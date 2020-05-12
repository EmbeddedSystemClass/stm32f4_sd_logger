/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "main.h"
#include "fatfs.h"
#include "string.h"
//DIR dirj;
FATFS   fatfs; 		/* FAT File System */
FRESULT fresult;  /* FAT File Result */
FIL			myfile;		/* FILE Instance */
DIR dir;
FILINFO fno;
uint8_t write_data[]="Hello!\nThis is my friend.";
uint32_t byte_cnt = 0;
uint8_t rx_buffer[100],byte_read;


/* Private variables ---------------------------------------------------------*/
SD_HandleTypeDef hsd;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_SDIO_SD_Init(void);
static void UART_PRINTF_Init(void);
float a=0.001567;

uint8_t buffer[20]="hjhjkl";

/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
UINT BytesWritten;
uint8_t file_name[10]="6t5.txt";
char* path;
	
	
char *dec32(unsigned long i) {
	static char str[16];
	char *s = str + sizeof(str);

	*--s = 0;

	do {
		*--s = '0' + (char) (i % 10);
		i /= 10;
	} while (i);

	return (s);
}
char str_dct[256];
float write_speed;
uint16_t m;
uint32_t start_cnt, stop_cnt;
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	UART_PRINTF_Init();
	printf("\n\r SD card SDIO 4 bit\n\r");
	printf("init SD card...!\n\r");
	if(BSP_SD_Init()==MSD_OK)
	{
		printf("init SD card succeed!\n\r");
		fresult=f_mount(&fatfs,"",1);
		HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B);
		/* Create File with name "stm32.txt" and file access is write */
		if (fresult != FR_OK) {
			Error_Handler();
		}
		printf("Mount SD card successfully\n\r");
		/*
			create a new file
		*/
		fresult = f_open(&myfile, (const TCHAR*)file_name, FA_CREATE_NEW);
		if (fresult != FR_OK) {
			if (fresult == FR_EXIST) {
				printf("file name is already exist!\n\r");
				printf("try to overridely create a new file\n\r");
				fresult = f_open(&myfile, (const TCHAR*)file_name, FA_CREATE_ALWAYS|FA_WRITE);
				if (fresult != FR_OK) {
					Error_Handler();
				}
				printf("override file ok\n\r");
			}
		}
		else {
			printf("created a new file ok\n\r");
		}
			
		printf("Open file successfully\n\r");
		
   // f_printf(&myfile,"sdadsadadsadxxxxxxxxxxxxxxxxxxxx\n\r");
		
		start_cnt = HAL_GetTick();
		char dum_str[256] = "zzzzzzzzzzzaaaaaaaaaaaaaaaaqqqqqqqqqqqqqqqqqqqqqqqqqqxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxzzzHelloworldcxzcxzcxzczczczcxzcxzcxzcxzcxzcxxxxxxxxxxsssssssssssaaaaaaqqqqqqqqqqq\n\r";
		//uint32_t byte_cnt;
		for (m = 0; m < 15000; m++) {
				fresult = f_write(&myfile, dum_str, strlen(dum_str), &BytesWritten);
				byte_cnt+=BytesWritten;
		}
		// flush cached data
		f_sync(&myfile);					
		f_close(&myfile);
		stop_cnt = HAL_GetTick();
		write_speed = (float)byte_cnt/((stop_cnt - start_cnt)*0.000001f);
		float write_speed_mb;
		write_speed_mb = write_speed/1024.0f/1024.0f/8.0f;
				printf("write speed in MB/s: %f\n\r", write_speed_mb);
/*
	list dir content code
	prior to calling this code, sd is mounted
*/
	fresult = f_open(&myfile, "DIR_LIST.TXT", FA_CREATE_ALWAYS | FA_WRITE);

		if (fresult == FR_OK) {
			UINT BytesWritten2;

			path = "";

			fresult = f_opendir(&dir, path);

			if (fresult == FR_OK) {
				while (1) {
					
					char *s = str_dct;
					char *fn;

					fresult = f_readdir(&dir, &fno);

					if ((fresult != FR_OK) || (fno.fname[0] == 0))
						break;

					fn = fno.fname;

					*s++ = ((fno.fattrib & AM_DIR) ? 'D' : '-');
					*s++ = ((fno.fattrib & AM_RDO) ? 'R' : '-');
					*s++ = ((fno.fattrib & AM_SYS) ? 'S' : '-');
					*s++ = ((fno.fattrib & AM_HID) ? 'H' : '-');

					*s++ = ' ';

					strcpy(s, dec32(fno.fsize));
					s += strlen(s);

					*s++ = ' ';

					strcpy(s, path);
					s += strlen(s);

					*s++ = '/';

					strcpy(s, fn);
					s += strlen(s);

					*s++ = 0x0D;
					*s++ = 0x0A;
					*s++ = 0;

					fresult = f_write(&myfile, str_dct, strlen(str_dct), &BytesWritten2);
					printf("%s", str_dct);
				}
			}
/**/
			
			
			fresult = f_close(&myfile); // DIR.TXT
			f_mount(NULL, "", 1);
		}
	}
	else {
		printf("init SD card failed!\n\r");
	}
	while (1)
	{
		printf("\n\r While loop\n\r");
		HAL_Delay(1500);
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
}
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */


static void UART_PRINTF_Init(void) {
	
  UartHandle.Instance          = USARTx;
  
  UartHandle.Init.BaudRate     = 38400;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler(); 
  }
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF); 

  return ch;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler */
	printf("\n\r SD card isnot OK, fresult=%d\n\r", fresult);
	/* User can add his own implementation to report the HAL error return state */
	while(1) 
	{
	}
	/* USER CODE END Error_Handler */ 
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/