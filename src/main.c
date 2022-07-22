#include "fw_hal.h"

uint16_t res;

INTERRUPT(ADC_Routine, EXTI_VectADC)
{
    ADC_ClearInterrupt();
    res = ADC_RESL;
    res |= (ADC_RES & 0x0F) << 8;
    // Restart ADC for continuous sampling
    ADC_Start();
}

int main()
{
    GPIO_P3_SetMode(GPIO_Pin_5, GPIO_Mode_Output_PP);
    GPIO_P3_SetMode(GPIO_Pin_4, GPIO_Mode_Output_PP);

    UART1_Config8bitUart(UART1_BaudSource_Timer2, HAL_State_ON, 115200);
    UART1_TxString("Legend-Tech\r\n");

    // PWMA2P P1.2
    // PWMA3P P1.4
    // PWMA4P P1.6
    // PWM5_2 P1.7 (PWMB1)
    // Set GPIO pins output mode
    GPIO_P1_SetMode(GPIO_Pin_2 | GPIO_Pin_4, GPIO_Mode_Output_PP);
    GPIO_P1_SetMode(GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Output_PP);
    // Turn off PWMA.1 before change its mode
    // Set PWMA.1 port direction output
    PWMA_PWM2_SetPortDirection(PWMA_PortDirOut);
    PWMA_PWM3_SetPortDirection(PWMA_PortDirOut);
    PWMA_PWM4_SetPortDirection(PWMA_PortDirOut);
    PWMB_PWM1_SetPortDirection(PWMB_PortDirOut);
    // Set PWMA.1 output low voltage when counter is less than target value
    PWMA_PWM2_ConfigOutputMode(PWM_OutputMode_PWM_LowIfLess);
    PWMA_PWM3_ConfigOutputMode(PWM_OutputMode_PWM_LowIfLess);
    PWMA_PWM4_ConfigOutputMode(PWM_OutputMode_PWM_LowIfLess);
    PWMB_PWM1_ConfigOutputMode(PWM_OutputMode_PWM_LowIfLess);
    // Enable comparison value preload to make duty cycle changing smooth
    PWMA_PWM2_SetComparePreload(HAL_State_ON);
    PWMA_PWM3_SetComparePreload(HAL_State_ON);
    PWMA_PWM4_SetComparePreload(HAL_State_ON);
    PWMB_PWM1_SetComparePreload(HAL_State_ON);

    // Turn on PWMA.1
    PWMA_PWM2_SetPortState(HAL_State_ON);
    PWMA_PWM3_SetPortState(HAL_State_ON);

    PWMA_PWM4_SetPortState(HAL_State_ON);

    PWMB_PWM1_SetPortState(HAL_State_ON);

    // Turn on PWMA.1 complementary output
    PWMA_PWM2_SetPortState(HAL_State_ON);
    PWMA_PWM3_SetPortState(HAL_State_ON);
    PWMA_PWM4_SetPortState(HAL_State_ON);
    PWMB_PWM1_SetPortState(HAL_State_ON);
    /**
     * PWM clock prescaler and auto-reload period
     *
     * side alignment:
     *      Fpwm = SYSCLK / (PWMx_PSCR + 1) / (PWMx_ARR + 1)
     * central alignment:
     *      Fpwm = SYSCLK / (PWMx_PSCR + 1) / PWMx_ARR / 2
     */
    // Set highest PWM clock
    PWMA_SetPrescaler(1);
    PWMB_SetPrescaler(1);

    // PWM width = Period + 1 (side alignment), or AutoReloadPeriod * 2 (center alignment)
    PWMA_SetPeriod(0xFFF);
    PWMB_SetPeriod(0xFFF);

    // Counter direction, down:from [PWMA_ARRH,PWMA_ARRL] to 0
    PWMA_SetCounterDirection(PWM_CounterDirection_Down);
    PWMB_SetCounterDirection(PWM_CounterDirection_Down);

    // Enable preload on reload-period
    PWMA_SetAutoReloadPreload(HAL_State_ON);
    PWMB_SetAutoReloadPreload(HAL_State_ON);

    // Enable output on PWMA.1P, PWMA.1N
    PWMA_SetPinOutputState(PWM_Pin_2 | PWM_Pin_3 | PWM_Pin_4, HAL_State_ON);
    PWMB_SetPinOutputState(PWM_Pin_1, HAL_State_ON);
    // Set PWMA.1 alternative ports to P1.0 and P1.1
    PWMA_PWM2_SetPort(PWMA_PWM2_AlterPort_P12P54_P13);
    PWMA_PWM3_SetPort(PWMA_PWM3_AlterPort_P14_P15);
    PWMA_PWM4_SetPort(PWMA_PWM4_AlterPort_P16_P17);
    PWMB_PWM1_SetPort(PWMB_PWM5_AlterPort_P17);
    // Enable overall output
    PWMA_SetOverallState(HAL_State_ON);
    PWMB_SetOverallState(HAL_State_ON);

    // Start counter
    PWMA_SetCounterState(HAL_State_ON);
    PWMB_SetCounterState(HAL_State_ON);

    PWMA_PWM2_SetCaptureCompareValue(0xfff + 1);
    PWMA_PWM3_SetCaptureCompareValue(0xfff + 1);
    PWMA_PWM4_SetCaptureCompareValue(0xfff + 1);
    PWMB_PWM1_SetCaptureCompareValue(0xfff + 1);

    // adc
    //  Set ADC1(GPIO P1.1) HIP
    GPIO_P1_SetMode(GPIO_Pin_1, GPIO_Mode_Input_HIP);
    // Channel: ADC1
    ADC_SetChannel(1);
    // ADC Clock = SYSCLK / 2 / (1+15) = SYSCLK / 32
    ADC_SetClockPrescaler(0x0F);
    // Right alignment, high 2-bit in ADC_RES, low 8-bit in ADC_RESL
    ADC_SetResultAlignmentRight();
    // Enable interrupts
    EXTI_Global_SetIntState(HAL_State_ON);
    EXTI_ADC_SetIntState(HAL_State_ON);
    // Turn on ADC power
    ADC_SetPowerState(HAL_State_ON);
    // Start ADC
    ADC_Start();
    // ccc
    int a = 1;
    while (1)
    {
        a++;
        P35 = RESET;
        P34 = SET;

        P35 = SET;
        P34 = RESET;

        UART1_TxHex(res >> 8);
        UART1_TxHex(res & 0xFF);
        UART1_TxString("\r\n");
        while (1)
        {
            /* code */
        }
        
        if (!P30)
        {
            IAP_CONTR = 0x60;
        }

        if (res > 0 && res <50 )
        {
            PWMA_PWM2_SetCaptureCompareValue(0xfff + 1);
        }
        else
        {
            PWMA_PWM2_SetCaptureCompareValue(0);
        }
    }

    return 0;
}
