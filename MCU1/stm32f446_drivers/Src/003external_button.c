/*
 * 003external_button.c
 *
 *  Created on: Oct 14, 2025
 *      Author: jiperez
 */

#include "stm32f446.h"

void delay(void)
{
	for(uint32_t i = 0; i < 500000/2; i++);
}

int main(void)
{
	GPIO_Handle_t GpioLed;
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Handle_t GpioButton;
	GpioButton.pGPIOx = GPIOC;
	GpioButton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GpioButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RT;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOC, ENABLE);
	GPIO_PeriClockControl(GPIOA, ENABLE);

	GPIO_Init(&GpioLed);
	GPIO_Init(&GpioButton);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI15_10,ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI15_10, 0);
	while(1)
	return 0;
}

void EXTI15_10_IRQHandler(void){
	GPIO_IRQHandling(13);
	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
}

