/*
 * ldr.h
 *
 *  Created on: Mar 22, 2026
 *      Author: Nhan
 */

#ifndef SOURCE_MIDDLE_LDR_LDR_H_
#define SOURCE_MIDDLE_LDR_LDR_H_

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_adc.h"
#include "em_iadc.h"
#include "em_gpio.h"

#define IADC_INPUT_0_BUS          CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_CDBUSALLOC_CDEVEN0_ADC0


void LDRInit(void);
uint32_t LightSensor_ReadADC(void);

#endif /* SOURCE_MIDDLE_LDR_LDR_H_ */
