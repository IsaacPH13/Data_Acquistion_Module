/*
 * 011DMA2.c
 *
 *  Created on: Oct 28, 2025
 *      Author: jiperez
 */
#include "stm32f446.h"

TIM_Handle_t *pTIM ;
GPIO_Handle_t *pGPIO;
ADC_Handle_t *pADC;
DMA_Handle_t *pDMA;
uint32_t value[4];
float new[4];
int main(void)
{
	#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));  // Full access CP10/CP11
	#endif

	SystemCLK_Config_84MHz();

	GPIO_Handle_t GpioLed;
	pGPIO = &GpioLed;
	GPIO_PeriClockControl(GPIOA, ENABLE);

	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioLed);

    GPIO_Handle_t ADCIn_0;
    ADCIn_0.pGPIOx = GPIOA;
    ADCIn_0.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
    ADCIn_0.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
    ADCIn_0.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIO_Init(&ADCIn_0);

    GPIO_Handle_t ADCIn_1;
    ADCIn_1.pGPIOx = GPIOA;
    ADCIn_1.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
    ADCIn_1.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
    ADCIn_1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIO_Init(&ADCIn_1);

    GPIO_Handle_t ADCIn_2;
    ADCIn_2.pGPIOx = GPIOA;
    ADCIn_2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
    ADCIn_2.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_4;
    ADCIn_2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIO_Init(&ADCIn_2);

    GPIO_Handle_t ADCIn_3;
    ADCIn_3.pGPIOx = GPIOA;
    ADCIn_3.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
    ADCIn_3.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
    ADCIn_3.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIO_Init(&ADCIn_3);

    ADC_Handle_t ADC_channel_0;
    pADC = &ADC_channel_0;
    ADC_PeriClockControl(ADC1, ENABLE);

    ADC_channel_0.pADCx = ADC1;
    ADC_channel_0.ADC_Config.ADC_Resolution = ADC_RESOLUTION_12_B;
    ADC_channel_0.ADC_Config.ADC_DataAlignment = ADC_DATA_ALIGNMENT_RIGHT;
    ADC_channel_0.ADC_Config.ADC_ScanMode =  ADC_SCAN_MODE_EN;
    ADC_channel_0.ADC_Config.ADC_ConversionMode =  ADC_CONV_MODE_SINGLE;
    ADC_channel_0.ADC_Config.ADC_ExternalTriggerDetection =  ADC_EXT_TRIG_DECT_RE;
    ADC_channel_0.ADC_Config.ADC_ExternalTrigger =  ADC_EXT_TRIG_TIM2_TRGO;
    ADC_channel_0.ADC_Config.ADC_DMAContinuousRequests =  ADC_DMA_MODE_EN;
    ADC_channel_0.ADC_Config.ADC_DDSelection =  ADC_DDS_RQ;
    ADC_channel_0.ADC_Config.ADC_EOCSelection =  ADC_EOC_PER_CONVERSION;
    ADC_channel_0.ADC_Config.ADC_EOCInterrupt =  ADC_EOC_IT_DI;

    ADC_channel_0.ADC_NumChannels = 4;
    ADC_ChannelConfig(&ADC_channel_0, 4, 0, ADC_SMP_T_15);
    ADC_ChannelConfig(&ADC_channel_0, 6, 1, ADC_SMP_T_15);
    ADC_ChannelConfig(&ADC_channel_0, 0, 2, ADC_SMP_T_15);
    ADC_ChannelConfig(&ADC_channel_0, 1, 3, ADC_SMP_T_15);
    ADC_ConfigSequence(&ADC_channel_0);
    ADC_Init(&ADC_channel_0);

    DMA_Handle_t DMA_0;
    pDMA = &DMA_0;

    DMA_0.pDMAx = DMA2;
    DMA_0.DMA_stream = 0;
    DMA_0.DMA_Config.DMA_Channel = DMA_CHANNEL_0;
    DMA_0.DMA_Config.DMA_Direction = DMA_DIR_PERIPH_TO_MEM;
    DMA_0.DMA_Config.DMA_Priority = DMA_PRIORITY_HIGH;
    DMA_0.DMA_Config.DMA_MemDataSize = DMA_DATA_SIZE_WORD;
    DMA_0.DMA_Config.DMA_PeriphDataSize = DMA_DATA_SIZE_WORD;
    DMA_0.DMA_Config.DMA_MemInc = ENABLE;
    DMA_0.DMA_Config.DMA_PeriphInc = DISABLE;
    DMA_0.DMA_Config.DMA_FIFOMode = DMA_FIFO_MODE_DISABLED;
    DMA_0.DMA_Config.DMA_FIFOThreshold = 0;
    DMA_0.DMA_Config.DMA_Mode = DMA_MODE_CIRCULAR;
    DMA_0.DMA_Config.DMA_TransferIT = ENABLE;
    DMA_0.BufferSize = 4;

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
	TIM_2.TIM_Config.TIM_IntEnable = TIM_IT_DISABLE;
	TIM_2.TIM_Config.TIM_MasterModeSel = TIM_MMS_UPDATE;
	TIM_Init(&TIM_2);

	TIM_Start(&TIM_2);
	DMA_IRQInterruptConfig(IRQ_NO_DMA2_STREAM0, ENABLE);
	DMA_IRQPriorityConfig(IRQ_NO_DMA2_STREAM0, 0);

	while(1);
	return 0;
}

void DMA2_Stream0_IRQHandler(void)
{
	DMA_IRQHandling(pDMA);
	for(__vo uint8_t i = 0; i<=3; i++) new[i] = value[i];
}




