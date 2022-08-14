#include "fw_hal.h"
#include "init.h"

#define R3 P13
#define R4 P15

extern uint16_t angle0_target;
extern uint16_t angle1_target;

void I2C_Init(void)
{
    GPIO_P3_SetMode(GPIO_Pin_3, GPIO_Mode_InOut_QBD);

    I2C_SetWorkMode(I2C_WorkMode_Slave);

    uint8_t address = 0x10;
    address = address | (R3 << 1) | R4;

    SFRX_ON();
    // i2c 从设备地址设为0x10（7 bit）
    // 该寄存器高7位是i2c从设备地址
    I2CSLADR = address << 1;
    SFRX_OFF();

    // 还是I2CSLADR，最低位是地址比较使能，置0后仅接受配置的地址，1接受所有地址
    // 接收了会自动回ack，并且进到中断
    I2C_SetSlaveAddrControl(0);

    // 如果地址匹配成功，会自动回
    // 回是回了，但实际上回的东西不一定是ack，而是由 SLACKO 这个配置位控制，为1是正常回ack，为0回nack
    // 发了ack后这个配置位不会被清空，所以只需要设置一次
    // 此处是坑点，文档没说清楚

    SFRX_ON();
    I2CSLST &= ~0x1; // SLACKO
    SFRX_OFF();

    I2C_SetPort(I2C_AlterPort_P32_P33);
    // 开启i2c
    I2C_SetEnabled(HAL_State_ON);

    // 开启各种中断
    EXTI_I2C_SetSlvStartIntState(HAL_State_ON);
    EXTI_I2C_SetSlvRecvIntState(HAL_State_ON);
    EXTI_I2C_SetSlvSendIntState(HAL_State_ON);
    EXTI_I2C_SetSlvStopIntState(HAL_State_ON);
    // 开启总中断
    EXTI_Global_SetIntState(HAL_State_ON);
}

INTERRUPT(I2C_Routine, EXTI_VectI2C)
{
    static uint8_t address;
    static uint8_t count;
    static uint16_t angle;
    // 访问下面这些寄存器要开这个开关
    SFRX_ON();

    if (I2CSLST & 0x40) // STAIF 收到start信号
    {
        I2CSLST &= ~0x40;
        I2CTXD = battery & 0xff;
    }
    else if (I2CSLST & 0x20) // RXIF  接受到一字节   接受到的是寻址的字节还是后续的数据，算这个
    {
        I2CSLST &= ~0x20; //处理RECV事件
        // I2CRXD保存了这次收到的字节
        if (!address)
        {
            address = I2CRXD;
            count = 0;
            I2CRXD = 0;
        }
        else
        {
            if (count < 4)
            {
                switch (count)
                {
                case 0:
                {
                    angle = I2CRXD;
                }
                break;
                case 1:
                {
                    angle = angle + I2CRXD * 256;
                    angle0_target = angle;
                }
                break;
                case 2:
                {
                    angle = I2CRXD;
                }
                break;
                case 3:
                {
                    angle = angle + I2CRXD * 256;
                    angle1_target = angle;
                }
                break;
                default:
                    break;
                }
                count++;
                I2CRXD = 0;
            }
        }
    }
    else if (I2CSLST & 0x10) // TXIF 发送完成
    {
        I2CSLST &= ~0x10;   //处理SEND事件
        if (I2CSLST & 0x02) // 是否接收到ack
        {
            //在这里修改I2CTXD
            I2CTXD = battery >> 8;
        }
        else
        {
            //在这里修改I2CTXD
            I2CTXD = battery >> 8;
        }
    }
    else if (I2CSLST & 0x08) //收到stop
    {
        I2CSLST &= ~0x08; //处理STOP事件
        address = 0;
        count = 0;
    }
    SFRX_OFF();
}

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

int main()
{
    init_led();
    init_adc();
    init_pwm();

    UART1_Config8bitUart(UART1_BaudSource_Timer2, HAL_State_ON, 115200);
    UART1_TxString("Legend Tech Simple Servo\r\n");

    GPIO_P1_SetMode(GPIO_Pin_3, GPIO_Mode_Input_HIP); // r3
    GPIO_P1_SetMode(GPIO_Pin_5, GPIO_Mode_Input_HIP); // r4

    I2C_Init();

    // while (1)
    // {
    // if (P13)
    // {
    //     led_blue_on();
    // }
    // else
    // {
    //     led_blue_off();
    // }

    // if (P15)
    // {
    //     led_red_on();
    // }
    // else
    // {
    //     led_red_off();
    // }
    // }

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
        UART1_TxHex(angle0_target >> 8);
        UART1_TxHex(angle0_target & 0xFF);
        UART1_TxString("   angle0\r\n");
        UART1_TxHex(angle1_target >> 8);
        UART1_TxHex(angle1_target & 0xFF);
        UART1_TxString("   angle1\r\n");
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
                    pwm_servo_0_backward_ex(d + 60);
                }
                // UART1_TxString("   backward\r\n");
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
                    pwm_servo_0_forward_ex(d + 60);
                }
                // UART1_TxString("   forward\r\n");
            }
        }
        else
        {
            pwm_servo_0_stop();
            // UART1_TxString("   stop\r\n");
        }
        if ((angle0 > 280) || (angle0 < 80))
        {
            pwm_servo_0_stop();
            // UART1_TxString("angle0   shutdown\r\n");
            // UART1_TxHex(angle0 >> 8);
            // UART1_TxHex(angle0 & 0xFF);
            // UART1_TxString("   angle0\r\n");
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
                    pwm_servo_1_backward_ex(d + 60);
                }
                // UART1_TxString("   backward\r\n");
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
                    pwm_servo_1_forward_ex(d + 60);
                }
                // UART1_TxString("   forward\r\n");
            }
        }
        else
        {
            pwm_servo_1_stop();
            // UART1_TxString("   stop\r\n");
        }
        if ((angle1 > 280) || (angle1 < 80))
        {
            pwm_servo_1_stop();
            // UART1_TxString("angle1   shutdown\r\n");
            // UART1_TxHex(angle1 >> 8);
            // UART1_TxHex(angle1 & 0xFF);
            // UART1_TxString("   angle1\r\n");
            // while (1)
            // {
            //     /* code */
            // }
        }
        //////////////////////// ////////////
    }

    return 0;
}
