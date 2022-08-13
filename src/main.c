#include "fw_hal.h"
#include "init.h"

uint16_t distance(uint16_t a, uint16_t b)
{
    if (a > b)
    {
        return a - b;
    }
    else
    {
        return b - a;
    }
}

uint16_t d = 0;
extern uint16_t angle0_target;
extern uint16_t angle1_target;

int main()
{
    init_led();
    init_adc();
    init_pwm();

    UART1_Config8bitUart(UART1_BaudSource_Timer2, HAL_State_ON, 115200);
    UART1_TxString("Legend-Techqqq\r\n");

    GPIO_P1_SetMode(GPIO_Pin_3, GPIO_Mode_Input_HIP);  //r3
    GPIO_P1_SetMode(GPIO_Pin_5, GPIO_Mode_Input_HIP); //r4

    while (1)
    {
        if (P13)
        {
            led_blue_on();
        }
        else
        {
            led_blue_off();
        }

        if (P15)
        {
            led_red_on();
        }
        else
        {
            led_red_off();
        }
    }

    while (1)
    {
        // uint16_t aaa = 0;
        // aaa = battery;
        // UART1_TxHex(battery >> 8);
        // UART1_TxHex(battery & 0xFF);
        // UART1_TxString("   battery\r\n");
        // UART1_TxHex(angle0 >> 8);
        // UART1_TxHex(angle0 & 0xFF);
        // UART1_TxString("   angle0\r\n");
        // UART1_TxHex(angle1 >> 8);
        // UART1_TxHex(angle1 & 0xFF);
        // UART1_TxString("   angle1\r\n");
        // pwm_servo_0_stop();

        // if (!P30)
        // {
        //     IAP_CONTR = 0x60;
        // }

        if (angle0 != angle0_target)
        {
            if (angle0 > angle0_target)
            {
                d = distance(angle0, angle0_target);
                if (d > 99)
                {
                    pwm_servo_0_backward();
                }
                else
                {
                    pwm_servo_0_backward_ex(d + 50);
                }
                UART1_TxString("   backward\r\n");
            }
            else
            {
                d = distance(angle0, angle0_target);
                if (d > 99)
                {
                    pwm_servo_0_forward();
                }
                else
                {
                    pwm_servo_0_forward_ex(d + 50);
                }
                UART1_TxString("   forward\r\n");
            }
        }
        else
        {
            pwm_servo_0_stop();
            UART1_TxString("   stop\r\n");
        }
        if ((angle0 > 270) || (angle0 < 90))
        {
            pwm_servo_0_stop();
            UART1_TxString("angle0   shutdown\r\n");
            UART1_TxHex(angle0 >> 8);
            UART1_TxHex(angle0 & 0xFF);
            UART1_TxString("   angle0\r\n");
            // while (1)
            // {
            //     /* code */
            // }
        }
        ////////////////////////////////////
        if (angle1 != angle1_target)
        {
            if (angle1 > angle1_target)
            {
                d = distance(angle1, angle1_target);
                if (d > 99)
                {
                    pwm_servo_1_backward();
                }
                else
                {
                    pwm_servo_1_backward_ex(d + 50);
                }
                UART1_TxString("   backward\r\n");
            }
            else
            {
                d = distance(angle1, angle1_target);
                if (d > 99)
                {
                    pwm_servo_1_forward();
                }
                else
                {
                    pwm_servo_1_forward_ex(d + 50);
                }
                UART1_TxString("   forward\r\n");
            }
        }
        else
        {
            pwm_servo_1_stop();
            UART1_TxString("   stop\r\n");
        }
        if ((angle1 > 270) || (angle1 < 90))
        {
            pwm_servo_1_stop();
            UART1_TxString("angle1   shutdown\r\n");
            UART1_TxHex(angle1 >> 8);
            UART1_TxHex(angle1 & 0xFF);
            UART1_TxString("   angle1\r\n");
            // while (1)
            // {
            //     /* code */
            // }
        }
        //////////////////////// ////////////
    }

    return 0;
}
