/*
 * 015PWMV.c
 *
 *  Created on: Oct 29, 2025
 *      Author: jiperez
 */
#include "stm32f446.h"

TIM_Handle_t *pTIM ;
GPIO_Handle_t *pGPIO;
ADC_Handle_t *pADC;
DMA_Handle_t *pDMA;
uint16_t value[4];
uint16_t new[4];
#define INC		82

TIM_Handle_t *pTIM5 ;
GPIO_Handle_t *pGPIO;
int main(void)
{
	SystemCLK_Config_84MHz();

	SCB_CPACR |= ((3UL << 10*2) | (3UL << 11*2));

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_PeriClockControl(GPIOC, ENABLE);

	GPIO_Handle_t GpioPWM;
	pGPIO = &GpioPWM;
	GPIO_PeriClockControl(GPIOA, ENABLE);

	GpioPWM.pGPIOx = GPIOC;
	GpioPWM.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GpioPWM.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
	GpioPWM.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioPWM.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioPWM.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioPWM);

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
    DMA_SetAddresses(&DMA_0,(void*)&ADC_channel_0.pADCx->DR,(void*)value);
    DMA_StartTransfer(&DMA_0);

	TIM_Handle_t TIM_2;
	pTIM = &TIM_2;
	TIM_2.pTIMx = TIM2;
	TIM_2.TIM_Config.TIM_Frequency = 9600;
	TIM_2.TIM_Config.TIM_CLKDivision = TIM_CKD_DIV1;
	TIM_2.TIM_Config.TIM_AutoReloadPreload = TIM_ARPE_ENABLE;
	TIM_2.TIM_Config.TIM_CNTMode = TIM_UPCOUNT_MODE;
	TIM_2.TIM_Config.TIM_IntEnable = TIM_IT_ENABLE;
	TIM_2.TIM_Config.TIM_MasterModeSel = TIM_MMS_UPDATE;
	TIM_Init(&TIM_2);

	TIM_Handle_t TIM_5;
	pTIM5 = &TIM_5;
	TIM_5.pTIMx = TIM5;
	TIM_5.TIM_Config.TIM_Frequency = 960000;
	TIM_5.TIM_Config.TIM_CLKDivision = TIM_CKD_DIV1;
	TIM_5.TIM_Config.TIM_AutoReloadPreload = TIM_ARPE_ENABLE;
	TIM_5.TIM_Config.TIM_CNTMode = TIM_UPCOUNT_MODE;
	TIM_5.TIM_Config.TIM_IntEnable = TIM_IT_ENABLE;
	TIM_5.TIM_Config.TIM_MasterModeSel = TIM_MMS_UPDATE;
	TIM_Init(&TIM_5);

	TIM_Start(&TIM_3);
	TIM_IRQInterruptConfig(IRQ_NO_TIM3, ENABLE);
	TIM_IRQPriorityConfig(IRQ_NO_TIM3, 0);

	TIM_Start(&TIM_2);
	TIM_IRQInterruptConfig(IRQ_NO_TIM2, ENABLE);
	TIM_IRQPriorityConfig(IRQ_NO_TIM2, 1);

	while(1);
	return 0;
}

void TIM2_IRQHandler(void)
{

	TIM_IRQHandling(pTIM);
	new[0] = value[0];
}

void TIM5_IRQHandler(void)
{
	static uint16_t cont = 0;
	static uint8_t dir = 0;
	static uint8_t current_state = 0;
	static uint8_t last_state = 0;
	TIM_IRQHandling(pTIM2);

	if((dir==0) && (cont<4095)) cont+=INC;
	else if((dir==0) && (cont>=4095)) dir = 1;

	if((dir==1) && (cont>0)) cont-=INC;
	else if((dir==1) && (cont<=0))
		{
			dir = 0;
			cont+=INC;
		}

	if(new[0] > cont) current_state = 1;
	else current_state = 0;

	if(current_state != last_state)
	{
		if(current_state)
			pGPIO->pGPIOx->BSRR = ( 1 << 7 );
		else
			pGPIO->pGPIOx->BSRR = ( 1 << ( 7 + 16 ) );

		last_state = current_state;
	}

}







