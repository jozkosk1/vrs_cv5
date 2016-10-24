/*
 * vrs_cv5.c
 *
 *  Created on: 17. 10. 2016
 *      Author: Jozo
 */
#include "vrs_cv5.h"
extern uint16_t hodnota;

void ADC1_IRQHandler(void)
{
	hodnota = ADC_GetConversionValue(ADC1);

	ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
	ADC_ClearITPendingBit(ADC1,ADC_IT_OVR);
}
