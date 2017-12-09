/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "bsp_led.h"
#include "bsp_usart1.h"
#include "my_extrn_value.h"
#include "wdz_eeprom.h"
#include "bsp_stm_flash.h"
#include "my_time.h"

#include "wdz_m35.h"
#include "my_OS.h"
#include "my_globle_extern.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
void MX_FREERTOS_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
//����ʹ��
uint16_t my_FLASH_rx_buf1[100]= {0};
uint16_t my_FLASH_tx_buf1[100]= {0};

extern  uint8_t my_UART1_Status;
extern uint16_t my_tim6_count;



//����1��
uint8_t my_usart1_tx_buf1[270]= {0x10,0x49,0x01,0x02,0x13,0x16};
uint8_t my_system_restart_status=0;  //ϵͳ������ʶ��1Ϊ������0Ϊ����


extern uint16_t DTU_ADDRESS; //DTU��ȫ�ֵ�ַ��2���ֽ� uint16_t MY_DTU_add
extern uint8_t my_sys_init_flash;

RTC_DateTypeDef my_RTC_date;
RTC_TimeTypeDef my_RTC_time;
uint8_t my_sys_verson[]="2017-10-16V001";//ϵͳ�汾��

/* USER CODE END 0 */

int main(void)
{

    /* USER CODE BEGIN 1 */
    __IO	uint8_t my_SPI_rx_buf1[100]= {0};
    __IO	uint8_t my_SPI_tx_buf1[100]= {0};

    int ii=0;

    /* USER CODE END 1 */

    /* MCU Configuration----------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART3_UART_Init();
    MX_SPI1_Init();
    MX_RTC_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_SPI3_Init();
    MX_TIM6_Init();

    /* USER CODE BEGIN 2 */
    HAL_NVIC_DisableIRQ(EXTI0_IRQn);
		//ϵͳ������ʶ
		my_system_restart_status=1;

    //====LED��SPeaker����
    LED1_ON;
    HAL_Delay(100);
    LED2_ON;
    HAL_Delay(100);
    LED3_ON;
    HAL_Delay(100);
    LED4_ON;
    HAL_Delay(100);
    LED5_ON;
    HAL_Delay(100);
    LED6_ON;
    HAL_Delay(1000);
    SPEAKER_ON;
   
    HAL_Delay(500);
    SPEAKER_OFF;
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
    LED4_OFF;
    LED5_OFF;
    LED6_OFF;
    //=======
    //ϵͳ�汾��
		printf("\n===========\n");
		printf("%s",my_sys_verson);
		printf("===========\n");
    

    //===��������
    USART_printf(&huart3,"\n123======system is reset!!=========123\n");
    USART_printf(&huart3,"===sysclockfreg==%d\n",HAL_RCC_GetSysClockFreq()/1000000);//��ȡϵͳʱ��
    HAL_UART_Receive_IT(&huart3,&rsbuf3[rsbuf3pt_write],1);  //��������USART3����

    HAL_UART_Receive_IT(&huart1,&rsbuf1[rsbuf1pt_write],1);  //��������1�ӽӣ���ӦM35
		HAL_UART_Receive_IT(&huart2,&rsbuf2[rsbuf2pt_write],1);  //��������1�ӽӣ���ӦM35

    //===SPI1 ��ӦEEPROM
    EEPROM_NSS1_OFF;
    EEPROM_NSS2_OFF;

    //���ֽ� ����EEPROM
//    for(ii=0; ii<76; ii++)
//    {
//        my_SPI_tx_buf1[ii]=ii+0X30;
//        my_FLASH_tx_buf1[ii]=ii+0X30;
//        AT25_WriteByte(my_SPI_tx_buf1[ii],ii);
//    }
//    for(ii=0; ii<76; ii++)
//    {
//        my_SPI_rx_buf1[ii]=AT25_ReadByte(ii);
//    }
//    USART_printf(&huart3,"[111]%s\n",my_SPI_rx_buf1);
//    //�����
//    SPI_EE_BufferRead2((uint8_t *)my_SPI_rx_buf1,1000,76);
//    USART_printf(&huart3,"[222]%s\n",my_SPI_rx_buf1);
//    SPI_EE_BufferWrite2((uint8_t *)my_SPI_tx_buf1,1000,76);
//    SPI_EE_BufferRead2((uint8_t *)my_SPI_rx_buf1,1000,76);
//    USART_printf(&huart3,"[333]%s\n",my_SPI_rx_buf1);
    //��ʼ������FLASH����
		MY_Value_init();
		
		
		
		
    //�ڲ�STM FLASH����

    STMFLASH_Read(FLASH_ReadAddress,my_FLASH_rx_buf1,100);
    printf("\nSTM-default read data\n");
    for(ii=0; ii<100; ii++)
    {
        printf("[%XH]-",my_FLASH_rx_buf1[ii]);
    }
    //my_SPI_rx_buf1[ii]=my_FLASH_rx_buf1[ii];
    //USART_printf(&huart3,"[STM-FLASH111=]%s\n",my_SPI_rx_buf1);
    //HAL_Delay(1000);

    STMFLASH_Write(FLASH_WriteAddress,my_FLASH_tx_buf1,76);
    HAL_Delay(1000);

    STMFLASH_Read(FLASH_ReadAddress,my_FLASH_rx_buf1,100);
    printf("\nSTM-write and read data\n");
    for(ii=0; ii<100; ii++)
    {
        printf("[%XH]-",my_FLASH_rx_buf1[ii]);
    }

    //TIM ��ʱ��ʼ



    HAL_TIM_Base_Stop_IT(&htim6);
    //����TIM6����ʱʹ��10ms����һ�Σ���ֵ��һ


    //====TIM END========

    //====RTC=====
		my_RTC_date.Date=6;
		my_RTC_date.Month=10;
		my_RTC_date.Year=17;
    HAL_RTC_SetDate(&hrtc,&my_RTC_date,RTC_FORMAT_BIN);
		
		my_RTC_time.Seconds=33;
		my_RTC_time.Minutes=25;
		my_RTC_time.Hours=13;
		HAL_RTC_SetTime(&hrtc,&my_RTC_time,RTC_FORMAT_BIN);

    //=====RTC END====


    //====SPI3 CC1101�ӿ�====
    //----CC1101


    HAL_GPIO_WritePin(PORT_CC_CSN,PIN_CC_CSN,GPIO_PIN_SET);
    HAL_Delay(2000);
    HAL_GPIO_WritePin(PORT_CC_CSN,PIN_CC_CSN,GPIO_PIN_RESET);





    printf("GDO2_CONFIG-%XH\n",CC1101ReadReg(CC1101_IOCFG2));
    printf("GDO1_CONFIG-%XH\n",CC1101ReadReg(CC1101_IOCFG1));
    printf("GDO0_CONFIG-%XH\n",CC1101ReadReg(CC1101_IOCFG0));

    CC1101Init();
    HAL_Delay(1000);

    printf("GDO2_CONFIG-%XH\n",CC1101ReadReg(CC1101_IOCFG2));
    printf("GDO1_CONFIG-%XH\n",CC1101ReadReg(CC1101_IOCFG1));
    printf("GDO0_CONFIG-%XH\n",CC1101ReadReg(CC1101_IOCFG0));

    my_read_all_reg();

    //__HAL_GPIO_EXTI_CLEAR_IT(PIN_CC_IRQ);
    //HAL_NVIC_EnableIRQ(EXIT_CC_IRQ_GD0);

    //--------����WORģʽ,��ǰ��ʽ�����ð�ť�жϣ�����WORģʽ
    //EVE0=0X876A T0Ϊ1�� ����T0Ϊ0.5���ʱ��Ϊ0X876A��һ�룬0X43B5H  0.25��0X21DA

    CC1101WriteCmd(CC1101_SIDLE); //�������״̬
    CC1101WriteReg(CC1101_MCSM2,0X00); //д0x16 ,RX_TIME��д��0X00�����ý���ʱ�����ƣ�ռ�ձ�����00���12%,���û���յ����ݣ��ͽ���SLEEP��
    CC1101WriteReg(CC1101_MCSM0,0x18);  //0X18 	//
    CC1101WriteReg(CC1101_WOREVT1,0x87);  //0X1E,event0 ���ֽ�
    CC1101WriteReg(CC1101_WOREVT0,0x6A);   //0X1F event0 ���ֽ�
    CC1101WriteReg(CC1101_WORCTRL,0X78); //д0X20,0111 100,WOR_RES
    //CC1101WriteCmd(CC1101_SWOR); //����WORģʽ

    //---------�������ģʽ

    CC1101SetIdle();
    CC1101WriteReg(CC1101_MCSM2,0X07); //д0x16 ,RX_TIME��д��0X07,����û��ʱ�����ƣ�һ�½�������
    CC1101SetTRMode(RX_MODE);           // �������ģʽ
    HAL_Delay(500);
    printf("after rx CC1101 status=[%XH] \n",CC1101ReadStatus(CC1101_MARCSTATE));//0X01���У�0X0D���գ�0X13����

    HAL_Delay(500);
    //---------��ʼ����ɣ����������жϣ�������ʱ

    HAL_TIM_Base_Start_IT(&htim6);  //TIM6 1�룬��ʱ��
    //HAL_NVIC_EnableIRQ(EXIT_CC_IRQ_GD0); //�����жϣ�����/�������ݲ���
//-------------



    //====SPI3 CC1101�ӿ� end===
#if USE_INIT_M35==1
    //M35��������
    //***�ֻ�ģ��M35����***
    M35_EMERG_OFF;
    M35_PWR_OFF;
    M35_PWREN_CONTROL_OFF; //M35�ϵ�״̬
    HAL_Delay(1000);
    M35_PWREN_CONTROL_ON;  //M35�ϵ磬��û�п���
    M35_PWR_ON;      //�ߵ�ƽ����3�����ң��������߹ػ�
    HAL_Delay(3000);
    M35_PWR_OFF;    //�����͵�ƽ�������������߹ػ�״̬��Ϊ�͵�ƽ
    /*
    ��ʼ��M35ģ��
    */
    //M35_PWREN_CONTROL_OFF; //��M35�ϵ�,����433ʱ����������ʹ�ã���ʽʹ�����ε�
    my_init_m35(); //����433ʱ�ر�

#endif
    //HAL_Delay(1000);
    //END

    my_speaker_long_voice(50);
    my_speaker_short_cycle_voice(4,25);
		
		//ϵͳ������ʶ
		my_system_restart_status=1;

    /* USER CODE END 2 */

    /* Call init function for freertos objects (in freertos.c) */
    MX_FREERTOS_Init();

    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        HAL_Delay(2000);
        //CC1101SetTRMode(RX_MODE);           // �������ģʽ
        //printf("main CC1101 status=[%XH] \n",CC1101ReadStatus(CC1101_MARCSTATE));

        LED1_TOGGLE;

        ii=my_at_senddata(my_usart1_tx_buf1);
        if(ii==1) printf("gprs is ok!=%d\n",my_usart1_tx_buf1[1]);
        else if(ii==0) printf("gprs is error!=%d\n",my_usart1_tx_buf1[1]);
        else
            printf("gprs is error GRPS=%d\n",ii);


        my_usart1_tx_buf1[1]++;
        if(my_UART1_Status==1)
        {
            MX_USART1_UART_Init();
            HAL_UART_Receive_IT(&huart1,&rsbuf1[rsbuf1pt_write],1);  //��������1����
            my_UART1_Status=0;
            printf("usart1 is error!!\r\n");
        }

//		LED2_TOGGLE;
//		LED3_TOGGLE;
//		LED4_TOGGLE;
//		LED5_TOGGLE;
//		LED6_TOGGLE;





        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

    }
    /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure the Systick interrupt time
    */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM8 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM8) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */

    if(htim==&htim6)
    {
			HAL_TIM_PeriodElapsedCallback2(htim);
        
//        my_tim6_count++; //1���ӵļ�ʱ��
//        if(my_tim6_count%7==0)
//        {
//           my_tim6_old_count=my_os_count1;
//        }
//				else if(my_tim6_count%13==0)
//				{
//					if(my_tim6_old_count==my_os_count1)
//					{
//						printf("\n  OS is stop \n");
//					}
//					
//				}			
//				else if(my_tim6_count%100==0)
//				{
//					my_fun_CC1101_init_reum();
//				}
//				LED6_TOGGLE;
    }

    /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {
    }
    /* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/