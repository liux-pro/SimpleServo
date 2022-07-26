#include "fw_hal.h"
#include "init.h"

int main()
{
    init_led();
    init_adc();


    UART1_Config8bitUart(UART1_BaudSource_Timer2, HAL_State_ON, 115200);
    UART1_TxString("Legend-Techqqq\r\n");

    while (1)
    {
        uint16_t aaa = 0;
        aaa = battery;
        UART1_TxHex(battery >> 8);
        UART1_TxHex(battery & 0xFF);
        UART1_TxString("   battery\r\n");
        UART1_TxHex(angle0 >> 8);
        UART1_TxHex(angle0 & 0xFF);
        UART1_TxString("   angle0\r\n");
        UART1_TxHex(angle1 >> 8);
        UART1_TxHex(angle1 & 0xFF);
        UART1_TxString("   angle1\r\n");
        SYS_Delay(500);
        // while (1)
        // {
        //     /* code */
        // }

        // if (!P30)
        // {
        //     IAP_CONTR = 0x60;
        // }

        // if (res > 0 && res <50 )
        // {
        //     PWMA_PWM2_SetCaptureCompareValue(0xfff + 1);
        // }
        // else
        // {
        //     PWMA_PWM2_SetCaptureCompareValue(0);
        // }
    }

    return 0;
}
