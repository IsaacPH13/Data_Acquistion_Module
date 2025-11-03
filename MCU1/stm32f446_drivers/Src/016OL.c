/*
 * 016OL.c
 *
 *  Created on: Nov 3, 2025
 *      Author: jiperez
 */
#include "stm32f446.h"

const float inc = 0.1;
TIM_Handle_t *pTIM2;
TIM_Handle_t *pTIM5;
GPIO_Handle_t *pGPIO;

ADC_Handle_t *pADC;
DMA_Handle_t *pDMA;
uint16_t sensors[4];
float v_cd;
float v_g;
float i_L;
float i_L90;
float i_inv;

//float cos;
//float sin;

float u_ol_pos;
float u_ol_neg;

uint8_t OPERATION_MODE = 0;

int main(void)
{
	SystemCLK_Config_84MHz();

	SCB_CPACR |= ((3UL << 10*2) | (3UL << 11*2));

	GPIO_PeriClockControl(GPIOA, ENABLE);

	GPIO_Handle_t GpioPWM;
	pGPIO = &GpioPWM;
	GPIO_PeriClockControl(GPIOC, ENABLE);

	GpioPWM.pGPIOx = GPIOC;
	GpioPWM.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GpioPWM.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
	GpioPWM.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GpioPWM.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioPWM.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioPWM);

	GPIO_Handle_t GpioPWMC;

	GpioPWMC.pGPIOx = GPIOA;
	GpioPWMC.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
	GpioPWMC.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
	GpioPWMC.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GpioPWMC.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioPWMC.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioPWMC);

	GPIO_Handle_t ADCIn_0;
	ADCIn_0.pGPIOx = GPIOA;
	ADCIn_0.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
	ADCIn_0.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	ADCIn_0.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&ADCIn_0);

	ADC_Handle_t ADC_channel_0;
	pADC = &ADC_channel_0;
	ADC_PeriClockControl(ADC1, ENABLE);

	ADC_channel_0.pADCx = ADC1;
	ADC_channel_0.ADC_Config.ADC_Resolution = ADC_RESOLUTION_12_B;
	ADC_channel_0.ADC_Config.ADC_DataAlignment = ADC_DATA_ALIGNMENT_RIGHT;
	ADC_channel_0.ADC_Config.ADC_ScanMode =  ADC_SCAN_MODE_DI;
	ADC_channel_0.ADC_Config.ADC_ConversionMode =  ADC_CONV_MODE_SINGLE;
	ADC_channel_0.ADC_Config.ADC_ExternalTriggerDetection =  ADC_EXT_TRIG_DECT_RE;
	ADC_channel_0.ADC_Config.ADC_ExternalTrigger =  ADC_EXT_TRIG_TIM2_TRGO;
	ADC_channel_0.ADC_Config.ADC_DMAContinuousRequests =  ADC_DMA_MODE_EN;
	ADC_channel_0.ADC_Config.ADC_DDSelection =  ADC_DDS_RQ;
	ADC_channel_0.ADC_Config.ADC_EOCSelection =  ADC_EOC_PER_CONVERSION;
	ADC_channel_0.ADC_Config.ADC_EOCInterrupt =  ADC_EOC_IT_DI;

	ADC_channel_0.ADC_NumChannels = 1;
	ADC_ChannelConfig(&ADC_channel_0, 0, 0, ADC_SMP_T_15);
	ADC_ConfigSequence(&ADC_channel_0);
	ADC_Init(&ADC_channel_0);

	DMA_Handle_t DMA_0;
	pDMA = &DMA_0;

	DMA_0.pDMAx = DMA2;
	DMA_0.DMA_stream = 0;
	DMA_0.DMA_Config.DMA_Channel = DMA_CHANNEL_0;
	DMA_0.DMA_Config.DMA_Direction = DMA_DIR_PERIPH_TO_MEM;
	DMA_0.DMA_Config.DMA_Priority = DMA_PRIORITY_HIGH;
	DMA_0.DMA_Config.DMA_MemDataSize = DMA_DATA_SIZE_HALFWORD;
	DMA_0.DMA_Config.DMA_PeriphDataSize = DMA_DATA_SIZE_HALFWORD;
	DMA_0.DMA_Config.DMA_MemInc = DISABLE;
	DMA_0.DMA_Config.DMA_PeriphInc = DISABLE;
	DMA_0.DMA_Config.DMA_FIFOMode = DMA_FIFO_MODE_DISABLED;
	DMA_0.DMA_Config.DMA_FIFOThreshold = 0;
	DMA_0.DMA_Config.DMA_Mode = DMA_MODE_CIRCULAR;
	DMA_0.BufferSize = 1;

	DMA_Init(&DMA_0);
	DMA_SetAddresses(&DMA_0,(void*)&ADC_channel_0.pADCx->DR,(void*)sensors);
	DMA_StartTransfer(&DMA_0);

	TIM_Handle_t TIM_2;
	pTIM2 = &TIM_2;
	TIM_2.pTIMx = TIM2;
	TIM_2.TIM_Config.TIM_Frequency = 9600;
	TIM_2.TIM_Config.TIM_CLKDivision = TIM_CKD_DIV1;
	TIM_2.TIM_Config.TIM_AutoReloadPreload = TIM_ARPE_ENABLE;
	TIM_2.TIM_Config.TIM_CNTMode = TIM_UPCOUNT_MODE;
	TIM_2.TIM_Config.TIM_IntEnable = TIM_IT_ENABLE;
	TIM_2.TIM_Config.TIM_MasterModeSel = TIM_MMS_UPDATE;
	TIM_Init(&TIM_2);

	TIM_IRQInterruptConfig(IRQ_NO_TIM2, ENABLE);
	TIM_IRQPriorityConfig(IRQ_NO_TIM2, 1);


	TIM_Handle_t TIM_5;
	pTIM5 = &TIM_5;
	//TIM_DeInit(TIM5);
	TIM_5.pTIMx = TIM5;
	TIM_5.TIM_Config.TIM_Frequency = 9600*40;
	TIM_5.TIM_Config.TIM_CLKDivision = TIM_CKD_DIV1;
	TIM_5.TIM_Config.TIM_AutoReloadPreload = TIM_ARPE_ENABLE;
	TIM_5.TIM_Config.TIM_CNTMode = TIM_UPCOUNT_MODE;
	TIM_5.TIM_Config.TIM_IntEnable = TIM_IT_ENABLE;
	TIM_5.TIM_Config.TIM_MasterModeSel = TIM_MMS_RESET;
	TIM_Init(&TIM_5);


	TIM_IRQInterruptConfig(IRQ_NO_TIM5, ENABLE);
	TIM_IRQPriorityConfig(IRQ_NO_TIM5, 0);


	/********************PWM_EN & LOOP_SEL********************/
	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Handle_t PWM_EN;
	PWM_EN.pGPIOx = GPIOB;
	PWM_EN.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	PWM_EN.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RFT;
	PWM_EN.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&PWM_EN);


	GPIO_Handle_t LOOP_SEL;
	LOOP_SEL.pGPIOx = GPIOB;
	LOOP_SEL.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	LOOP_SEL.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RFT;
	LOOP_SEL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&LOOP_SEL);

	GPIO_IRQInterruptConfig(IRQ_NO_EXTI15_10,ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI15_10, 0);

	TIM_Start(&TIM_2);
	//TIM_Start(&TIM_5);

	while(1);
	return 0;
}

void TIM5_IRQHandler(void)
{
	static __vo float cont = -1.1;
	static __vo uint8_t dir = 0;
	static __vo uint8_t current_state[2] = {0};
	static __vo uint8_t last_state[2] = {0};

	TIM_IRQHandling(pTIM5);

	if((dir==0) && (cont<1.0)) cont+=inc;
	else if((dir==0) && (cont>=1.0)) dir = 1;

	if((dir==1) && (cont>-1.0)) cont-=inc;
	else if((dir==1) && (cont<=0))
		{
			dir = 0;
			cont+=inc;
		}

	if(u_ol_pos > cont) current_state[0] = 1;
	else current_state[0] = 0;

	if(u_ol_neg > cont) current_state[1] = 1;
	else current_state[1] = 0;

	if(current_state[0] != last_state[0])
	{
		if(current_state[0])
			GPIOC->BSRR = ( 1 << 7 );
		else
			GPIOC->BSRR = ( 1 << ( 7 + 16 ) );
		last_state[0] = current_state[0];
	}

	if(current_state[1] != last_state[1])
	{
		if(current_state[1])
			GPIOA->BSRR = ( 1 << 9 );
		else
			GPIOA->BSRR = ( 1 << ( 9 + 16 ) );
		last_state[1] = current_state[1];
	}

}


void TIM2_IRQHandler(void)
{
	TIM_IRQHandling(pTIM2);
	if(OPERATION_MODE == 0)
	{
		u_ol_pos = (sensors[0]/4095.0f - 0.5f)*2.0f;
		u_ol_neg = (sensors[0]/4095.0f - 0.5f)*(-2.0f);
	} else
	{
		u_ol_pos = 0;
		u_ol_neg = 0;

	}
}

void EXTI15_10_IRQHandler(void)
{
	static uint8_t PWM_ENABLE = 0;
	GPIO_IRQHandling(14);
	PWM_ENABLE = GPIO_ReadFromInputPin(GPIOB, 14);
	OPERATION_MODE = GPIO_ReadFromInputPin(GPIOB, 15);
	if( PWM_ENABLE==0 )
	{
		TIM_Stop(pTIM5);
		GPIOC->BSRR = ( 1 << ( 7 + 16 ) );
		GPIOA->BSRR = ( 1 << ( 9 + 16 ) );

	} else if( PWM_ENABLE==1 )
	{
		TIM_Start(pTIM5);
	}

}


