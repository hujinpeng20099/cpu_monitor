#include "psu_thread.h"



/*record set digit value */
uint16_t set_voltage=0;
uint16_t set_current=0;
uint16_t adc_value[3];
/*digit value pointer */
int8_t set_value_ptr=7;
uint8_t ec11_btn=0;
/*output parameter */
uint16_t vout=1050,iout=1520;
uint8_t load_pre=100;

int8_t encoder_value=0;

uint8_t out_status=0;
/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef psu_adc;
DAC_HandleTypeDef psu_dac;
UART_HandleTypeDef dbg_uart;
UART_HandleTypeDef pd_uart;

void F071_Peripheral_Init(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_ADC_Init();
	MX_DAC_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
}	

void PSU_Window(void)
{
	LCD_DrawString(0,0,"V=00.00  I=0.000 OFF",FONT8,1);
	LCD_DrawString(0,8,"00.00V   0.000A",FONT16,1);
	LCD_DrawString(96,24,"0%",FONT8,0);
	LCD_DrawBar(0,24,load_pre,0);
}


/* 线程 1 的入口函数 */ 
void GUI_Entry(void *parameter)
{
	LCD_Init();
	PSU_Window();
	while (1)
	{
		set_value_update();
		set_couser_pos(set_value_ptr);
		Out_Value_Update(adc_value);
		LCD_refreshGram();
		rt_thread_mdelay(500);		
	}		
}

/* 线程 2 的入口函数 */ 
void HW_Refresh_Entry(void *parameter)
{
	uint16_t adc_cal_value=0;
	uint8_t i=0;
	
	/* start dac */
	HAL_DAC_Start(&psu_dac,DAC_CHANNEL_1);
	HAL_DAC_Start(&psu_dac,DAC_CHANNEL_2);
	rt_thread_mdelay(10);	
	HAL_DAC_SetValue(&psu_dac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,2000);
	HAL_DAC_SetValue(&psu_dac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,2000);
	/* cal adc */
	HAL_ADCEx_Calibration_Start(&psu_adc);
	rt_thread_mdelay(10);	
	adc_cal_value=HAL_ADC_GetValue(&psu_adc);
	rt_kprintf("ADC Cali Value=%d\r\n",adc_cal_value);
	while(1)
	{
		rt_thread_mdelay(120);
		if(LL_GPIO_IsInputPinSet(EC11_BTN_GPIO_Port,EC11_BTN_Pin)==0)
			{	
				rt_thread_mdelay(50);
				set_value_ptr--;
				if(set_value_ptr<0)set_value_ptr=7;
			}
		if(set_value_ptr>=4)
		{
			set_voltage=set_voltage+encoder_value*mypow(10,set_value_ptr-4);
			if(set_voltage>4000)set_voltage=4000;
		}
		else 
		{
			set_current=set_current+encoder_value*mypow(10,set_value_ptr);
			if(set_current>5000)set_current=5000;
		}
		
		for(i=0;i<3;i++)
		{
			HAL_ADC_Start(&psu_adc);
			if(HAL_ADC_PollForConversion(&psu_adc,ADC_CONV_TIMEOUT)==HAL_OK)
			{
				adc_value[i]=HAL_ADC_GetValue(&psu_adc);
				rt_kprintf("ADC Value%d=%d\r\n",i,adc_value[i]);
			}			
			
		}
		HAL_ADC_Stop(&psu_adc);
		encoder_value=0;			
	}
	
}


/* creat gui thread */
void PSU_Thread_Creat(void)
{	
	/*gui thread */
	rt_thread_t gui_tid = RT_NULL;	
	rt_thread_t HW_init = RT_NULL;
	/* 创建线程 1，名称是 gui，入口是 gui_entry*/
	gui_tid = rt_thread_create("gui_tid",
													GUI_Entry, RT_NULL,
													THREAD_STACK_SIZE,
													THREAD_PRIORITY, THREAD_TIMESLICE);

	/* 如果获得线程控制块，启动这个线程 */
	if (gui_tid != RT_NULL)rt_thread_startup(gui_tid);
	
	/* 创建线程 2，名称是 HW_init，入口是 HW_init_entry*/
	HW_init = rt_thread_create("HW_init",
													HW_Refresh_Entry, RT_NULL,
													THREAD_STACK_SIZE,
													THREAD_PRIORITY-1, THREAD_TIMESLICE);

	/* 如果获得线程控制块，启动这个线程 */
	if (HW_init != RT_NULL)rt_thread_startup(HW_init);
}



/**
  * @brief set_value_update
  * @param None
  */
void set_value_update(void)
{
	LCD_DrawNum(12,0,set_voltage/100,2,1);
	LCD_DrawNum(30,0,set_voltage%100,2,1);
	
	LCD_DrawNum(66,0,set_current/1000,1,1);
	LCD_DrawNum(78,0,set_current%1000,3,1);
}


/**
  * @brief Out_Value_Update
  * @param adc_buf
  */

void Out_Value_Update(uint16_t* adc_buf)
{
	LCD_DrawNum16(0,8,adc_buf[1]/100,2,1);
	LCD_DrawNum16(24,8,adc_buf[1]%100,2,1);
	
	LCD_DrawNum16(72,8,adc_buf[2]/1000,1,1);
	LCD_DrawNum16(88,8,adc_buf[2]%1000,3,1);

	if(out_status)LCD_DrawString(101,0," CV",FONT8,1);
	else LCD_DrawString(101,0,"OFF",FONT8,1);
}

/**
  * @brief set_couser_pos
  * @param cur_pos
  */
void set_couser_pos(uint8_t cur_pos)
{
	uint8_t conv_t=0;
	if(cur_pos>=4)conv_t=set_voltage/mypow(10,cur_pos-4)%10;
	else conv_t=set_current/mypow(10,cur_pos)%10;
	
	switch(cur_pos)
	{
		case 7:
			LCD_DrawNum(12,0,conv_t,1,0);
			break;
		case 6:
			LCD_DrawNum(18,0,conv_t,1,0);
			break;
		case 5:
			LCD_DrawNum(30,0,conv_t,1,0);
			break;
		case 4:
			LCD_DrawNum(36,0,conv_t,1,0);
			break;
		case 3:
			LCD_DrawNum(66,0,conv_t,1,0);
			break;
		case 2:
			LCD_DrawNum(78,0,conv_t,1,0);
			break;
		case 1:
			LCD_DrawNum(84,0,conv_t,1,0);
			break;
		case 0:
			LCD_DrawNum(90,0,conv_t,1,0);
			break;
		default:
			rt_kprintf("Set_Couser_Pos error!\r\n");
			break;
	}

}
/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  psu_adc.Instance = ADC1;
  psu_adc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  psu_adc.Init.Resolution = ADC_RESOLUTION_12B;
  psu_adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  psu_adc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  psu_adc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  psu_adc.Init.LowPowerAutoWait = DISABLE;
  psu_adc.Init.LowPowerAutoPowerOff = DISABLE;
  psu_adc.Init.ContinuousConvMode = ENABLE;
  psu_adc.Init.DiscontinuousConvMode = DISABLE;
  psu_adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  psu_adc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  psu_adc.Init.DMAContinuousRequests = DISABLE;
  psu_adc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&psu_adc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted. 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  if (HAL_ADC_ConfigChannel(&psu_adc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted. 
  */
  sConfig.Channel = ADC_CHANNEL_6;
  if (HAL_ADC_ConfigChannel(&psu_adc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted. 
  */
  sConfig.Channel = ADC_CHANNEL_7;
  if (HAL_ADC_ConfigChannel(&psu_adc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */
  /** DAC Initialization 
  */
  psu_dac.Instance = DAC;
  if (HAL_DAC_Init(&psu_dac) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config 
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&psu_dac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT2 config 
  */
  if (HAL_DAC_ConfigChannel(&psu_dac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  dbg_uart.Instance = USART1;
  dbg_uart.Init.BaudRate = 115200;
  dbg_uart.Init.WordLength = UART_WORDLENGTH_8B;
  dbg_uart.Init.StopBits = UART_STOPBITS_1;
  dbg_uart.Init.Parity = UART_PARITY_NONE;
  dbg_uart.Init.Mode = UART_MODE_TX_RX;
  dbg_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  dbg_uart.Init.OverSampling = UART_OVERSAMPLING_16;
  dbg_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  dbg_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&dbg_uart) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  pd_uart.Instance = USART2;
  pd_uart.Init.BaudRate = 115200;
  pd_uart.Init.WordLength = UART_WORDLENGTH_8B;
  pd_uart.Init.StopBits = UART_STOPBITS_1;
  pd_uart.Init.Parity = UART_PARITY_NONE;
  pd_uart.Init.Mode = UART_MODE_RX;
  pd_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  pd_uart.Init.OverSampling = UART_OVERSAMPLING_16;
  pd_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  pd_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&pd_uart) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}


/* USER CODE BEGIN 4 */
/* 移植控制台，实现控制台输出, 对接 rt_hw_console_output */
void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    __HAL_UNLOCK(&dbg_uart);

    size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            HAL_UART_Transmit(&dbg_uart, (uint8_t *)&a, 1, 1);
        }
        HAL_UART_Transmit(&dbg_uart, (uint8_t *)(str + i), 1, 1);
    }
}

/* 移植 FinSH，实现命令行交互, 需要添加 FinSH 源码，然后再对接 rt_hw_console_getchar */
/* 查询方式 */
char rt_hw_console_getchar(void)
{
    int ch = -1;

    if (__HAL_UART_GET_FLAG(&dbg_uart, UART_FLAG_RXNE) != RESET)
    {
        ch = dbg_uart.Instance->RDR & 0xff;
    }
    else
    {
        if(__HAL_UART_GET_FLAG(&dbg_uart, UART_FLAG_ORE) != RESET)
        {
            __HAL_UART_CLEAR_OREFLAG(&dbg_uart);
        }
        rt_thread_mdelay(10);
    }
    return ch;
}
/* USER CODE END 4 */

