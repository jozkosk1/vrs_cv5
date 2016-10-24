/*
 * vrs_cv5.c
 *
 *  Created on: 17. 10. 2016
 *      Author: Jozo
 */
#include "vrs_cv5.h"
extern uint16_t ADC_hodnota;
extern uint8_t p;
extern uint16_t sprava[6];
extern uint8_t i;


void ADC1_IRQHandler (void){
	if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
	{
		ADC_hodnota = ADC_GetConversionValue(ADC1);
	}
}


void USART2_IRQHandler (void){
static uint16_t temp;
		if (USART_GetFlagStatus(USART2, USART_FLAG_TC))
		{
			if (i == 0)
			{
				temp = ADC_hodnota*1000/4095*33;
				sprava[0] 	= 	temp;
				sprava[1] 	= 	0;
				sprava[2] 	= 	0;
				sprava[3] 	= 	0;
				sprava[4] 	= 	0;
				sprava[5] 	= 	0;
				sprava[6] 	= 	0;
			/*
				sprava[0] 	= 	(temp/10000) + 0x30;
				sprava[1] 	= 	0x2E;
				sprava[2] 	= 	(temp/1000)  + 0x30;
				sprava[3] 	= 	(temp/100)   + 0x30;
				sprava[4] 	= 	0x56;
				sprava[5] 	= 	'\n';
				sprava[6] 	= 	'\r';*/
			}


			USART_SendData(USART2, sprava[i]);
			i++;
			if (i >= 7)	i=0;

		}
}











void UART_init (void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/*konfiguracia USART*/
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate 				= 9600;
	USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 				= USART_StopBits_1;
	USART_InitStructure.USART_Parity 				= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl 	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 					= USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);
}




void NVIC_init(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);								//prioritna skupina 4

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel 						= ADC1_IRQn;		//vektor prerusenia
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 16;				//priorita cislo 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;				//subpriorita 0
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;			//povolenie prerusenia

	NVIC_Init(&NVIC_InitStructure);

	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);										//povolenie prerusenia na EOC
	ADC_ITConfig(ADC1, ADC_IT_OVR, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel 						= USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;

	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			//povolenie prerusenia na RXNE
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);			//povolenie prerusenia na TXE
}

void GPIO_init(void)
{
	 GPIO_InitTypeDef	GPIO_InitStruct;

	 GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_0;			//ADC na pine 0
	 GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AN;			//pin 0 ako analogovy
	 GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL;		//ziadny Pull na pine 0
	 GPIO_Init(GPIOA, &GPIO_InitStruct);

	 RCC_HSICmd(ENABLE);								//HSI oscilator zapnuty

	 /* Check that HSI oscillator is ready */
	 while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

	 GPIO_InitTypeDef gpioInitStruct;

	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	 gpioInitStruct.GPIO_Mode 	= GPIO_Mode_AF;					//alternativna funkcia
	 gpioInitStruct.GPIO_OType 	= GPIO_OType_PP;				//PushPull
	 gpioInitStruct.GPIO_Pin 	= GPIO_Pin_2 | GPIO_Pin_3;		//USART2 na pine 2 a 3
	 gpioInitStruct.GPIO_Speed 	= GPIO_Speed_40MHz;				//rychlost 30MHz
	 gpioInitStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL;				//ziadny Pull

	 GPIO_Init(GPIOA, &gpioInitStruct);

	 GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);	//nastavenie alternativnej funkcie na pinoch 2 a 3
	 GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

}


void ADC_init (void)
{
	 	 ADC_InitTypeDef	ADC_InitStruct;
		 /* Enable ADC clock */
		 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);		//hodiny pre ADC1 zapnute

		 /* Initialize ADC structure */
		 ADC_StructInit(&ADC_InitStruct);

		 /* ADC1 configuration */
		 ADC_InitStruct.ADC_Resolution 				= ADC_Resolution_12b;					//rozlisenie ADC 12 bitov
		 ADC_InitStruct.ADC_ContinuousConvMode 		= ENABLE;								//kontinualny prevod zapnuty
		 ADC_InitStruct.ADC_ExternalTrigConvEdge 	= ADC_ExternalTrigConvEdge_None;		//externy trigger vypnuty
		 ADC_InitStruct.ADC_DataAlign 				= ADC_DataAlign_Right;					//zarovnanie dat vpravo
		 ADC_InitStruct.ADC_NbrOfConversion 		= 1;									//pocet prevodov

		 ADC_Init(ADC1, &ADC_InitStruct);

		/* ADCx regular channel8 configuration */
		 ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_384Cycles);		//nastavenie ADC kanala

		 /* Enable the ADC */
		 ADC_Cmd(ADC1, ENABLE);																//zapnutie periferie

		 /* Wait until the ADC1 is ready */
		 while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
		 {
		 }
		 /* Start ADC Software Conversion */
		 ADC_SoftwareStartConv(ADC1);
}
