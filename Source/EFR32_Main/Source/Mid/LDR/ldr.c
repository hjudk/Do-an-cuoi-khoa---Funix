/*
 * ldr.c
 *
 *  Created on: Mar 22, 2026
 *      Author: Nhan
 */
#include "app/framework/include/af.h"
#include <math.h>
#include "ldr.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_adc.h"
#include "em_iadc.h"
#include "em_gpio.h"

void LDRInit(void)
{
    IADC_Init_t init = IADC_INIT_DEFAULT;
    IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
    IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
    IADC_SingleInput_t input = IADC_SINGLEINPUT_DEFAULT;

    // Bật clock và reset ADC
    CMU_ClockEnable(cmuClock_IADC0, true);
    IADC_reset(IADC0);

    // Chọn clock cho ADC
    CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);


    initAllConfigs.configs[0].reference    = iadcCfgReferenceVddx;
    initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed32x;
    initAllConfigs.configs[0].adcClkPrescale =
    		IADC_calcAdcClkPrescale(IADC0,
                                1000000UL,
                                0,
                                iadcCfgModeNormal,
                                init.srcClkPrescale);

    // Chọn chân input (PC5)
    input.posInput = iadcPosInputPortCPin5;
    input.negInput = iadcNegInputGnd;

    // Khởi tạo ADC
    IADC_init(IADC0, &init, &initAllConfigs);

    // Khởi tạo chế độ đọc single
    IADC_initSingle(IADC0, &initSingle, &input);

    // Route chân GPIO vào ADC
    GPIO->IADC_INPUT_0_BUS |= GPIO_CDBUSALLOC_CDODD0_ADC0;
}

uint32_t LightSensor_ReadADC(void)
{
    IADC_Result_t result;

    // Bắt đầu chuyển đổi đơn
    IADC_command(IADC0, iadcCmdStartSingle);

    // Đợi có dữ liệu (SINGLEFIFODV = 1)
    while (!(IADC0->STATUS & IADC_STATUS_SINGLEFIFODV));

    //Đọc dữ liệu từ FIFO
    result = IADC_pullSingleFifoResult(IADC0);

    return result.data;
}



