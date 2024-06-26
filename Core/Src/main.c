/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define HAL_UART_Transmit_DMA_STATIC(first, static_arr) do { HAL_UART_Transmit_DMA((first), static_arr, sizeof(static_arr)); } while(0)

#define M_USART6_CR1 (volatile uint32_t*)(uintptr_t) (USART6_BASE + 0x00) // Control register 1
#define M_USART6_CR2 (volatile uint32_t*)(uintptr_t) (USART6_BASE + 0x04) // Control register 2
#define M_USART6_BRR (volatile uint32_t*)(uintptr_t) (USART6_BASE + 0x0C) // Baud rate register
#define M_USART6_ISR (volatile uint32_t*)(uintptr_t) (USART6_BASE + 0x1C) // Interrupt and status register
#define M_USART6_TDR (volatile uint32_t*)(uintptr_t) (USART6_BASE + 0x28) // Transmit data register

#define UART_BAUDRATE 115200
#define FREQUENCY 25000000
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
  const	uint8_t b = 0xB2;
	uint8_t cat1[] = 
"                        _\n"
"                       | \\\n"
"                       | |\n"
"                       | |\n"
"  |\\                   | |\n"
" /, ~\\                / /\n"
"X     `-.....-------./ /\n"
" ~-. ~  ~              |\n"
"    \\             /    |\n"
"     \\  /_     ___\\   /\n"
"     | /\\ ~~~~~   \\ |\n"
"     | | \\        || |\n"
"     | |\\ \\       || )\n"
"    (_/ (_/      ((_/\n";

	uint8_t cat2[] =
"       _\n"
"       \\`*-.\n"                 
"        )  _`-.\n"                
"       .  : `. .\n"                
"       : _   '  \\\n"
"       ; *` _.   `*-._\n"         
"       `-.-'          `-.\n" 
"         ;       `       `.\n"    
"         :.       .        \\\n"  
"         . \\  .   :   .-'   .\n"
"         '  `+.;  ;  '      :\n"
"         :  '  |    ;       ;-.\n"
"         ; '   : :`-:     _.`* ;\n"
"      .*' /  .*' ; .*`- +'  `*'\n"
"      `*-*   `*-*  `*-*';\n";


uint8_t cat3[] = 
"                                               .--.\n"
"                                               `.  \\\n"
"                                                 \\  \\\n"
"                                                  .  \\\n"
"                                                  :   .\n"
"                                                  |    .\n"
"                                                  |    :\n"
"                                                  |    |\n"
"  ..._  ___                                       |    |\n"
" `."".`''''""--..___                              |    |\n"
" ,-\\  \\             ""-...__         _____________/    |\n"
" / ` \" '                    `\"\"\"\"\"\"\"\"                  .\n"
" \\                                                      L\n"
" (>                                                      \\\n"
"/                                                         \\\n"
"\\_    ___..---.                                            L\n"
"  `--'         '.                                           \\\n"
"                 .                                           \\_\n"
"                _/`.                                           `.._\n"
"             .'     -.                                             `.\n"
"            /     __.-Y     /''''''-...___,...--------.._            |\n"
"           /   _.\"    |    /                ' .      \\   '---..._    |\n"
"          /   /      /    /                _,. '    ,/           |   |\n"
"          \\_,'     _.'   /              /''     _,-'            _|   |\n"
"                  '     /               `-----''               /     |\n"
"                  `...-'                                       `...-'\n";

uint8_t sonic[] =
"          .,\n"
".      _,'f----.._\n"
"|\\ ,-'\"/  |     ,'\n"
"|,_  ,--.      /\n"
"/,-. ,'`.     (_\n"
"f  o|  o|__     \"`-.\n"
",-._.,--'_ `.   _.,-`\n"
"`\"' ___.,'` j,-'\n"
"  `-.__.,--'";

	uint8_t letters[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', '\n'};
	uint8_t cmd_set_cyrillic[] = {0x1B, 0x50, 0x02};
	uint8_t cmd_set_ibm[] = {0x1B, 0x50, 0x01};
	uint8_t message[] = "test\n";
	
	
	
	
	
	
	
	
	
	
	
	
	

	
	
	
	uint8_t new_line [] = " \n";
	uint8_t data_reset[] = {0x1B, 0x40};
	uint8_t data_print[] = {0x1B, 0x4A, 0xF4};
	uint8_t data_init[] = {0x1B, 0x5B, 0x09, 0x0A};
	uint8_t data_set_mode[] = {0x1B, 0x21, 0x09};
	uint8_t data_begin_print[] = {0x0D, 0x0A};
	uint8_t data_end[] = {0x1B, 0x56, 0x58};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_UART_IDLE_Callback (UART_HandleTypeDef *huart);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void uart_send_byte(char sym)
{
    // Wait for TXE:
    while ((*M_USART6_ISR & (1U <<  7U)) == 0U);

    // Put data into DR:
    *M_USART6_TDR = sym;
}

void print_string(const char* buf)
{
    for (size_t i = 0; buf[i] != '\0'; i++)
    {
        uart_send_byte(buf[i]);
    }
    //uart_send_byte('\n');
    //uart_send_byte('\r');
}

void seek_string_end(char* buf)
{
  size_t i;
  for (size_t i = 0; buf[i] != '\0'; i++){}
      
  buf[i+1] = '\n';
}

#define BUFSIZE 100 // здесь указать размер буфера нужного размера
uint8_t buffer[BUFSIZE] = {0,};

void M_CLEAN_BUF(char* buf)
{
  for (int i = 0; i < BUFSIZE; i++)
  {
      buf[i] = 0;
  }
}

// Функция обробатывает IDLE флаг.  Для того чтобы эта функци работала , нужно  добавить  изменения в
// Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_uart.h
// Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c


void HAL_UART_IDLE_Callback(UART_HandleTypeDef *huart) {
    //print_string("IDLE CALLBACK");
    if (huart == &huart6) {
        // Функция вызываетс если буфер не доконца заполнен
        __HAL_UART_DISABLE_IT(&huart6, UART_IT_IDLE);
        //HAL_UART_Transmit_IT(&huart6, buffer, BUFSIZE);
        // uint8_t res = HAL_UART_Transmit_IT(&huart6, (uint8_t *) "ERR_LEN", 7);
        // if (res == HAL_ERROR) {
        //     HAL_UART_Transmit(&huart6, (uint8_t *) "ERR_HAL", 7, 1000);
        // } else if (res == HAL_BUSY) {
        //     HAL_UART_Transmit(&huart6, (uint8_t *) "ERR_BUSY", 8, 1000);
        // } else if (res == HAL_OK) {

        // } else {
        //     HAL_UART_Transmit(&huart6, (uint8_t *) "ERR_ALL", 7, 1000);
        // }
        //HAL_UART_Transmit_IT(&huart6, buffer, BUFSIZE);
        HAL_UART_AbortReceive(&huart6);
        __HAL_UART_CLEAR_IDLEFLAG(&huart6);
        __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
        HAL_UART_Receive_IT(&huart6, (uint8_t *) buffer, BUFSIZE);
        seek_string_end(buffer);

        HAL_UART_Transmit_DMA_STATIC(&huart7, buffer);
        print_string(buffer);
        M_CLEAN_BUF(buffer);
    }
}

// /////////////////////////////////// полный буфер ///////////////////////////////////////
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//     if (huart == &huart6) {
//         // Функция вызывается если буфер заполнен, здесь мы обрабатываем  и парсим то что пришло
//         __HAL_UART_DISABLE_IT(&huart6, UART_IT_IDLE);
//         if (strcmp((char *) buffer, "1-0") == 0) {
//             HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
//             HAL_UART_Transmit_IT(&huart6, (uint8_t *) "OK", 2);

//         } else if (strcmp((char *) buffer, "1-1") == 0) {
//             HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
//             HAL_UART_Transmit_IT(&huart6, (uint8_t *) "OK", 2);

//         } else if (strcmp((char *) buffer, "2-0") == 0) {
//             HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
//             HAL_UART_Transmit_IT(&huart6, (uint8_t *) "OK", 2);

//         } else if (strcmp((char *) buffer, "2-1") == 0) {
//             HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
//             HAL_UART_Transmit_IT(&huart6, (uint8_t *) "OK", 2);

//         } else if (strcmp((char *) buffer, "3-0") == 0) {
//             HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
//             HAL_UART_Transmit_IT(&huart6, (uint8_t *) "OK", 2);

//         } else if (strcmp((char *) buffer, "3-1") == 0) {
//             HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
//             HAL_UART_Transmit_IT(&huart6, (uint8_t *) "OK", 2);

//         } else if (strcmp((char *) buffer, "4-0") == 0) {
//             HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
//             HAL_UART_Transmit_IT(&huart6, (uint8_t *) "OK", 2);

//         } else if (strcmp((char *) buffer, "4-1") == 0) {
//             HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
//             HAL_UART_Transmit_IT(&huart6, (uint8_t *) "OK", 2);

//         } else {
//             HAL_UART_Transmit_IT(&huart6, buffer, BUFSIZE);
//         }

//         HAL_UART_AbortReceive(&huart6);
//         __HAL_UART_CLEAR_IDLEFLAG(&huart6);
//         __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
//         //HAL_UART_Receive_IT(&huart6, (uint8_t *) buffer, BUFSIZE);
//     }
// }


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    // Здесь мы обрабатываем ошибку  отправки UART
    if (huart == &huart6) {
        __HAL_UART_DISABLE_IT(&huart6, UART_IT_IDLE);
        uint32_t er = HAL_UART_GetError(&huart6);
        HAL_UART_Abort_IT(&huart6);

        switch (er) {
            case HAL_UART_ERROR_PE:
                HAL_UART_Transmit(&huart6, (uint8_t *) "ERR_PARITY", 10, 1000);
                __HAL_UART_CLEAR_PEFLAG(&huart6);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
                break;

            case HAL_UART_ERROR_NE:
                HAL_UART_Transmit(&huart6, (uint8_t *) "ERR_NOISE", 9, 1000);
                __HAL_UART_CLEAR_NEFLAG(&huart6);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
                break;

            case HAL_UART_ERROR_FE:
                HAL_UART_Transmit(&huart6, (uint8_t *) "ERR_FRAME", 9, 1000);
                __HAL_UART_CLEAR_FEFLAG(&huart6);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
                break;

            case HAL_UART_ERROR_ORE:
                HAL_UART_Transmit(&huart6, (uint8_t *) "ERR_OVERRUN", 11, 1000);
                __HAL_UART_CLEAR_OREFLAG(huart);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
                break;

            case HAL_UART_ERROR_DMA:
                HAL_UART_Transmit(&huart6, (uint8_t *) "ERR_DMA", 7, 1000);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
                break;
            default:
                break;
        }
    }

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
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
  MX_DMA_Init();
  MX_UART7_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
	
	uint8_t buff[1] = {' '};
	
	// HAL_UART_Transmit_DMA(&huart7, cat3, sizeof(cat3));
	//HAL_UART_Transmit_DMA_STATIC(&huart7, message);

   // Включаем флаг IDLE
  __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
  // Включаем прерывание
  // HAL_UART_Receive_IT(&huart6, (uint8_t *) buffer, BUFSIZE);

  // print_string("\n");

  //print_string("INITED");
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_UART_Receive_IT(&huart6, (uint8_t *) buffer, BUFSIZE);

    //HAL_UART_Transmit_DMA_STATIC(&huart7, buffer);
    //M_CLEAN_BUF(buffer);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
