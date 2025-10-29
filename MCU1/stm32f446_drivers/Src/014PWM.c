/*
 * 014PWM.c
 *
 *  Created on: Oct 29, 2025
 *      Author: jiperez
 */
#include "stm32f446.h"

#define INC		82
#define DC		2048
TIM_Handle_t *pTIM ;
GPIO_Handle_t *pGPIO;
int main(void)
{
	SystemCLK_Config_84MHz();

	SCB_CPACR |= ((3UL << 10*2) | (3UL << 11*2));

	GPIO_Handle_t GpioPWM;
	pGPIO = &GpioPWM;
	GPIO_PeriClockControl(GPIOA, ENABLE);

	GpioPWM.pGPIOx = GPIOA;
	GpioPWM.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GpioPWM.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
	GpioPWM.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioPWM.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioPWM.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioPWM);

	TIM_Handle_t TIM_5;
	pTIM = &TIM_5;
	TIM_5.pTIMx = TIM5;
	TIM_5.TIM_Config.TIM_Frequency = 960000;
	TIM_5.TIM_Config.TIM_CLKDivision = TIM_CKD_DIV1;
	TIM_5.TIM_Config.TIM_AutoReloadPreload = TIM_ARPE_ENABLE;
	TIM_5.TIM_Config.TIM_CNTMode = TIM_UPCOUNT_MODE;
	TIM_5.TIM_Config.TIM_IntEnable = TIM_IT_ENABLE;
	TIM_5.TIM_Config.TIM_MasterModeSel = TIM_MMS_UPDATE;
	TIM_Init(&TIM_5);

	TIM_Start(&TIM_5);
	TIM_IRQInterruptConfig(IRQ_NO_TIM5, ENABLE);
	TIM_IRQPriorityConfig(IRQ_NO_TIM5, 0);

	while(1);
	return 0;
}

void TIM5_IRQHandler(void)
{
	static uint16_t cont = 0;
	static uint8_t dir = 0;
	static uint8_t current_state = 0;
	static uint8_t last_state = 0;
	TIM_IRQHandling(pTIM);

	if((dir==0) && (cont<4095)) cont+=INC;
	else if((dir==0) && (cont>=4095)) dir = 1;

	if((dir==1) && (cont>0)) cont-=INC;
	else if((dir==1) && (cont<=0))
		{
			dir = 0;
			cont+=INC;
		}

	if(DC > cont) current_state = 1;
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

