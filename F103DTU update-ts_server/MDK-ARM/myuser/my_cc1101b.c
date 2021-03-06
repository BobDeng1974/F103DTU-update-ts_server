#include "my_cc1101.h"
#define RF_reg_count 47
#define my_CC1101_default_channer 0x02    //芯片默认CC1101信道
uint8_t my_CC1101_chip_address=0x01;  //芯片地址，默认为0x05
uint8_t my_cc1101_dest_address=0xFE;
uint16_t my_CC1101_sync_word=0x0102;  //CC1101同步字，默认为0x8799
uint8_t my_CC1101_change_channer=my_CC1101_default_channer;  //动态信道号



//10, 7, 5, 0, -5, -10, -15, -20, dbm output power, 0x12 == -30dbm
INT8U PaTabel[] = { 0xc0, 0xC8, 0x84, 0x60, 0x68, 0x34, 0x1D, 0x0E};


extern uint8_t MY_CC1101_STATUS;
extern uint8_t Cmd_CC1101_RX[];


static const uint8_t CC1101InitData[RF_reg_count][2]=
{   /*
    {CC1101_IOCFG2,      0x0E},  //
    {CC1101_IOCFG1,      0x2E},
    {CC1101_IOCFG0,      0x07},  //
    {CC1101_FIFOTHR,     0x47},  //默认47，数量溢出
    {CC1101_PKTCTRL1,    0x06},  //06
    {CC1101_PKTCTRL0,    0x05},  //
    {CC1101_CHANNR,      0x01},  //
    {CC1101_FSCTRL1,     0x06},  //
    {CC1101_FREQ2,       0x0F},
    {CC1101_FREQ1,       0x62},
    {CC1101_FREQ0,       0x76},
    {CC1101_MDMCFG4,     0xF6},
    {CC1101_MDMCFG3,     0x43},
    {CC1101_MDMCFG2,     0x13},
    {CC1101_DEVIATN,     0x15},
    {CC1101_MCSM0,       0x18},  //
    {CC1101_FOCCFG,      0x16},
    {CC1101_WORCTRL,     0xFB},  //
    {CC1101_FSCAL3,      0xE9},
    {CC1101_FSCAL2,      0x2A},
    {CC1101_FSCAL1,      0x00},
    {CC1101_FSCAL0,      0x1F},
    {CC1101_TEST2,       0x81},
    {CC1101_TEST1,       0x35},
    {CC1101_MCSM1,       0x3B},//============
    */
    {CC1101_IOCFG2,      0x0E},
    {CC1101_IOCFG1,      0x2E},
    {CC1101_IOCFG0,      0x06},  //0X06，无数据时为0，有数据位高电平。0X46，无数据为高电平，有数据位低电平
    {CC1101_FIFOTHR,     0x4E},
    {CC1101_SYNC1,       0x87},
    {CC1101_SYNC0,       0x99},
    {CC1101_PKTLEN,      0xFF},
    {CC1101_PKTCTRL1,    0x06},  //0x06开启地址校验，04，不开启地址校验
    {CC1101_PKTCTRL0,    0x05},  //05 0000 0101
    {CC1101_ADDR,        0x05},
    {CC1101_CHANNR,      my_CC1101_default_channer},//信道
    {CC1101_FSCTRL1,     0x06},
    {CC1101_FSCTRL0,     0x00},
    {CC1101_FREQ2,       0x0F},
    {CC1101_FREQ1,       0x62},
    {CC1101_FREQ0,       0x76},
    {CC1101_MDMCFG4,     0xF6},
    {CC1101_MDMCFG3,     0x83},
    {CC1101_MDMCFG2,     0x13},  //默认为0X13
    {CC1101_MDMCFG1,     0x22},
    {CC1101_MDMCFG0,     0xF8},
    {CC1101_DEVIATN,     0x15},
    {CC1101_MCSM2,       0x07},
    {CC1101_MCSM1,       0x30},
    {CC1101_MCSM0,       0x18},
    {CC1101_FOCCFG,      0x16},
    {CC1101_BSCFG,       0x6C},
    {CC1101_AGCCTRL2,    0x03},
    {CC1101_AGCCTRL1,    0x40},
    {CC1101_AGCCTRL0,    0x91},
    {CC1101_WOREVT1,     0x87},
    {CC1101_WOREVT0,     0x6B},
    {CC1101_WORCTRL,     0xFB},
    {CC1101_FREND1,      0x56},
    {CC1101_FREND0,      0x10},
    {CC1101_FSCAL3,      0xE9},
    {CC1101_FSCAL2,      0x2A},
    {CC1101_FSCAL1,      0x00},
    {CC1101_FSCAL0,      0x1F},
    {CC1101_RCCTRL1,     0x41},
    {CC1101_RCCTRL0,     0x00},
    {CC1101_FSTEST,      0x59},
    {CC1101_PTEST,       0x7F},
    {CC1101_AGCTEST,     0x3F},
    {CC1101_TEST2,       0x81},
    {CC1101_TEST1,       0x35},
    {CC1101_TEST0,       0x0B},
};


// 串口相关变量
INT8U   COM_TxNeed = 0;
INT8U   COM_TimeOut = 0;
INT8U   COM_RxCounter = 0;
INT8U   COM_TxCounter = 0;
INT8U   COM_RxBuffer[65] = {0};
INT8U   COM_TxBuffer[65] = {0};

/*===========================================================================
* 函数 ：RF_RecvHandler() => 无线数据接收处理                               *
============================================================================*/
void RF_RecvHandler(void)
{
    INT8U length=0, recv_buffer[64]= {0};
    uint8_t i=0;

    //__HAL_GPIO_EXTI_CLEAR_IT(PIN_CC_IRQ); //这个可以清除外部中断，每个中断的清除函数都不一样，需要分别调用
    // HAL_NVIC_EnableIRQ(EXIT_CC_IRQ_GD0);
    CC1101SetTRMode(RX_MODE);           // 进入接收模式



    if (1 == CC_IRQ_READ())             // 检测无线模块是否产生接收中断
    {
        while (CC_IRQ_READ() == 1);

        // 读取接收到的数据长度和数据内容
        length = CC1101RecPacket(recv_buffer);

        // 判断接收数据是否正确
        if (length <= SEND_MAX && length>0)
        {
            LED3_ON;               // LED闪烁，用于指示收到数据
            for(i=0; i<length; i++)
            {
                printf("[%X]--",recv_buffer[i]);
            }
            printf("\n");
            //printf("%s\n",recv_buffer);
            LED3_OFF;

        }
    }
}

/*===========================================================================
* 函数 : BSP_RF_SendPacket() => 无线发送数据函数，串口接收数据，转发给CC1101                        *
============================================================================*/
void RF_SendPacket(void)
{
    INT8U i=0,j=0, length=0, buffer[65]= {0};

    if ((0==COM_TimeOut) && (COM_RxCounter>0) )
    {
        if(MY_CC1101_STATUS==0x01 || MY_CC1101_STATUS== 0x02) return;
        else MY_CC1101_STATUS=0X01;

        //HAL_NVIC_DisableIRQ(EXIT_CC_IRQ_GD0); //关闭中断，不响应发射中断
        CC1101SetIdle();

        //LED3_ON;

        length = COM_RxCounter;

        COM_RxCounter = 0;

        for (i=0; i<length; i++)   {
            buffer[i] = COM_RxBuffer[i];
        }

        //CC1101SendPacket(buffer, length, ADDRESS_CHECK);    // 发送数据，默认单地址发送方式
        for(j=0; j<=0x6; j++)			 //注意，频率的计算方法，基频+通道号*间隔带宽，所以要考虑，有可能超出CC1101的频率范围，导致CC1101死机
        {
            my_CC1101_change_channer=j;
            CC1101WriteReg(CC1101_CHANNR,my_CC1101_change_channer); //变换信道
            buffer[0]=j+0X30;  //调试使用，作为信道号的标记


            for(i=0xFA; i<0XFF; i++) //多地址发送，注意0地址是广播地址,变换地址发送
            {
                buffer[length]='-';
                buffer[length+1]=i-0XFA+0X30;
                buffer[length+2]='\n';
                CC1101SendPacket_add(buffer, length+3, ADDRESS_CHECK,i);
            }
        }

        //	HAL_NVIC_ClearPendingIRQ(EXIT_CC_IRQ_GD0); //清除历史中断请求信息 EXIT_CC_IRQ_GD0，这个不起作用，不知道为什么


        // __HAL_GPIO_EXTI_CLEAR_IT(PIN_CC_IRQ); //这个可以清除外部中断，每个中断的清除函数都不一样，需要分别调用
        //	HAL_NVIC_EnableIRQ(EXIT_CC_IRQ_GD0);


        // CC1101SetTRMode(RX_MODE);                           // 进入接收模式

        printf("USART TO CC1101\n");

        MY_CC1101_STATUS=0x00;
    }
}

void RF_SendPacket2(void)
{
    INT8U i=0, length=0, buffer[65]= {0};

    // if ((0==COM_TimeOut) && (COM_RxCounter>0))
    {


        LED3_ON;

        length = COM_RxCounter;

        COM_RxCounter = 0;
        length=5;
        COM_RxBuffer[0]=0x31;
        COM_RxBuffer[1]=0x32;
        COM_RxBuffer[2]=0x33;
        COM_RxBuffer[3]=0x34;
        COM_RxBuffer[4]=0x35;
        COM_RxBuffer[5]=0x36;

        for (i=0; i<length; i++)   {
            buffer[i] = COM_RxBuffer[i];
        }

        CC1101SendPacket(buffer, length, ADDRESS_CHECK);    // 发送数据
        CC1101SetTRMode(RX_MODE);                           // 进入接收模式

        HAL_Delay(2500);

        LED3_OFF;
    }
}




/*===========================================================================
* 函数 ：RF_Initial() => 初始化RF芯片                                       *
* 说明 ：CC1101的操作，已经被建成C库，见CC1101.c文件， 提供SPI和CSN操作，	*
         即可调用其内部所有函数用户无需再关心CC1101的寄存器操作问题。       *
============================================================================*/
void RF_Initial(void)
{
    CC1101Init();               // 初始化CC1101寄存器
    CC1101SetTRMode(RX_MODE);   // 接收模式
}

/*
================================================================================
Function : CC1101RecPacket( )
    Receive a packet
INPUT    : rxBuffer, A buffer store the received data
OUTPUT   : 1:received count, 0:no data
================================================================================
*/
INT8U RS_buf_status[2]= {0}; //用来存储接收到的数据的RSSI和crc结果

INT8U CC1101RecPacket( INT8U *rxBuffer )
{

    INT8U pktLen;
//    INT16U x;
    //j = 0;

    if ( CC1101GetRXCnt( ) != 0 )
    {
        pktLen = CC1101ReadReg(CC1101_RXFIFO);           // Read length byte
        //printf("rx_count=%d  length=%d \n",	CC1101GetRXCnt(),pktLen); //测试使用



        if( ( CC1101ReadReg( CC1101_PKTCTRL1 ) & ~0x03 ) != 0 )
        {
            //  x = CC1101ReadReg(CC1101_RXFIFO);  //这个字节是地址，用来消耗掉地址的1个字节

        }
        if( pktLen == 0 )           {
            return 0;
        }
        else                      {
            ;
        } // { pktLen --; }
        CC1101ReadMultiReg(CC1101_RXFIFO, rxBuffer, pktLen); // Pull data
        CC1101ReadMultiReg(CC1101_RXFIFO, RS_buf_status, 2);   // Read  status bytes

        CC1101ClrRXBuff( );

        if( RS_buf_status[1] & CRC_OK ) {
            return pktLen;
        }
        else                     {
            return 0;
        }
    }
    else
    {
        return 0;    // Error
    }
}


/*
================================================================================
Function : CC1101SendPacket( )
    Send a packet
INPUT    : txbuffer, The buffer stores data to be sent
           size, How many bytes should be sent
           mode, Broadcast or address check packet
OUTPUT   : None
================================================================================
*/
void CC1101SendPacket( INT8U *txbuffer, INT8U size, TX_DATA_MODE mode )
{
    INT8U address;

    HAL_NVIC_DisableIRQ(EXIT_CC_IRQ_GD0); //关闭中断，不产生发射中断

    if( mode == BROADCAST )             {
        address = 0;
    }
    else if( mode == ADDRESS_CHECK )    {
        address = CC1101ReadReg( CC1101_ADDR );
    }

    CC1101ClrTXBuff( );

    address=my_cc1101_dest_address; //测试使用，写入接收方地址
    if( ( CC1101ReadReg( CC1101_PKTCTRL1 ) & ~0x03 ) != 0 )
    {
        CC1101WriteReg( CC1101_TXFIFO, size + 1 );
        CC1101WriteReg( CC1101_TXFIFO, address );
    }
    else
    {
        CC1101WriteReg( CC1101_TXFIFO, size );
    }

    CC1101WriteMultiReg( CC1101_TXFIFO, txbuffer, size );
    CC1101SetTRMode( TX_MODE );

    while( HAL_GPIO_ReadPin(PORT_CC_IRQ,PIN_CC_IRQ) != 1 );
    while( HAL_GPIO_ReadPin(PORT_CC_IRQ,PIN_CC_IRQ) == 1 );
    //HAL_Delay(1000);



    CC1101ClrTXBuff( );  //情况发送FIFO
}


/*
================================================================================
Function : CC1101ClrTXBuff( )
    Flush the TX buffer of CC1101
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101ClrTXBuff( void )
{
    CC1101SetIdle();//MUST BE IDLE MODE
    CC1101WriteCmd( CC1101_SFTX );
}

/*
================================================================================
Function : CC1101SetIdle( )
    Set the CC1101 into IDLE mode
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101SetIdle( void )
{
    CC1101WriteCmd(CC1101_SIDLE);
}

/*
================================================================================
Function : CC1101ClrRXBuff( )
    Flush the RX buffer of CC1101
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101ClrRXBuff( void )
{
    CC1101SetIdle();//MUST BE IDLE MODE
    CC1101WriteCmd( CC1101_SFRX );
}

/*
================================================================================
Function : CC1101ReadMultiReg( )
    Read some bytes from the rigisters continously
INPUT    : addr, The address of the register
           buff, The buffer stores the data
           size, How many bytes should be read
OUTPUT   : None
================================================================================
*/
void CC1101ReadMultiReg( INT8U addr, INT8U *buff, INT8U size )
{
    INT8U i, j;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | READ_BURST);
    for( i = 0; i < size; i ++ )
    {
        for( j = 0; j < 20; j ++ );
        *( buff + i ) = SPI_ExchangeByte( 0xFF );
    }
    CC_CSN_HIGH( );
}



/*
================================================================================
Function : CC1101GetRXCnt( )
    Get received count of CC1101
INPUT    : None
OUTPUT   : How many bytes hae been received
================================================================================
*/
INT8U CC1101GetRXCnt( void )
{
    return ( CC1101ReadStatus( CC1101_RXBYTES )  & BYTES_IN_RXFIFO );
}

/*
================================================================================
Function : CC1101SetTRMode( )
    Set the device as TX mode or RX mode
INPUT    : mode selection
OUTPUT   : None
================================================================================
*/
void CC1101SetTRMode( TRMODE mode )
{
    if( mode == TX_MODE )
    {
        CC1101WriteReg(CC1101_IOCFG0,0x06);
        CC1101WriteCmd( CC1101_STX );
    }
    else if( mode == RX_MODE )
    {
        CC1101WriteReg(CC1101_IOCFG0,0x06);
        CC1101WriteCmd( CC1101_SRX );
    }
}



/*
================================================================================
Function : CC1101Init( )
    Initialize the CC1101, User can modify it
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101Init( void )
{
    volatile uint8_t i, j;

    CC1101Reset( );

    for( i = 0; i < RF_reg_count; i++ )  //写了23个寄存器 RF_reg_count
    {
        CC1101WriteReg( CC1101InitData[i][0], CC1101InitData[i][1] );
    }


    CC1101SetAddress( my_CC1101_chip_address, BROAD_0AND255 );
    CC1101SetSYNC( my_CC1101_sync_word );//0X8799
    CC1101WriteReg(CC1101_MDMCFG1,   0x72); //Modem Configuration

    CC1101WriteMultiReg(CC1101_PATABLE, PaTabel, 8 );

    i = CC1101ReadStatus( CC1101_PARTNUM );//for test, must be 0x80
    printf("PARTNUM=[%X]  ",i);
    i = CC1101ReadStatus( CC1101_VERSION );//for test, refer to the datasheet
    printf("VERSION=[%X]  \n",i);
}

/*
================================================================================
Function : CC1101ReadStatus( )
    Read a status register
INPUT    : addr, The address of the register
OUTPUT   : the value read from the status register
================================================================================
*/
INT8U CC1101ReadStatus( INT8U addr )
{
    INT8U i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | READ_BURST);
    i = SPI_ExchangeByte( 0xFF );
    CC_CSN_HIGH( );
    return i;
}



/*
================================================================================
Function : CC1101WriteMultiReg( )
    Write some bytes to the specified register
INPUT    : addr, The address of the register
           buff, a buffer stores the values
           size, How many byte should be written
OUTPUT   : None
================================================================================
*/
void CC1101WriteMultiReg( INT8U addr, INT8U *buff, INT8U size )
{
    INT8U i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | WRITE_BURST );
    for( i = 0; i < size; i ++ )
    {
        SPI_ExchangeByte( *( buff + i ) );
    }
    CC_CSN_HIGH( );
}




/*
================================================================================
Function : CC1101SetSYNC( )
    Set the SYNC bytes of the CC1101
INPUT    : sync, 16bit sync
OUTPUT   : None
================================================================================
*/
void CC1101SetSYNC( INT16U sync )
{
    CC1101WriteReg(CC1101_SYNC1, 0xFF & ( sync>>8 ) );
    CC1101WriteReg(CC1101_SYNC0, 0xFF & sync );
}


/*
================================================================================
Function : CC1101SetAddress( )
    Set the address and address mode of the CC1101
INPUT    : address, The address byte
           AddressMode, the address check mode
OUTPUT   : None
================================================================================
*/
void CC1101SetAddress( uint8_t address, ADDR_MODE AddressMode)
{
    uint8_t btmp = CC1101ReadReg( CC1101_PKTCTRL1 ) & ~0x03;
    CC1101WriteReg(CC1101_ADDR, address);
    if     ( AddressMode == BROAD_ALL )     {}
    else if( AddressMode == BROAD_NO  )     {
        btmp |= 0x01;
    }
    else if( AddressMode == BROAD_0   )     {
        btmp |= 0x02;
    }
    else if( AddressMode == BROAD_0AND255 ) {
        btmp |= 0x03;
    }
}


/*
================================================================================
Function : CC1101ReadReg( )
    read a byte from the specified register
INPUT    : addr, The address of the register
OUTPUT   : the byte read from the rigister
================================================================================
*/
uint8_t CC1101ReadReg( uint8_t addr )
{
    uint8_t i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | READ_SINGLE);
    i = SPI_ExchangeByte( 0xFF );
    CC_CSN_HIGH( );
    return i;
}


/*
================================================================================
Function : CC1101Reset( )
    Reset the CC1101 device
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101Reset( void )
{
    uint8_t x;

    CC_CSN_HIGH( );
    HAL_Delay(50);
    CC_CSN_LOW( );
    HAL_Delay(50);
    CC_CSN_HIGH( );
    for( x = 0; x < 100; x ++ );
    CC1101WriteCmd( CC1101_SRES );
}

void CC_CSN_LOW(void)
{

    HAL_GPIO_WritePin(PORT_CC_CSN,PIN_CC_CSN,GPIO_PIN_RESET);
    while( HAL_GPIO_ReadPin(PORT_CC_CSN,PIN_CC_CSN)!=GPIO_PIN_RESET);

}
void CC_CSN_HIGH( void)
{
    HAL_GPIO_WritePin(PORT_CC_CSN,PIN_CC_CSN,GPIO_PIN_SET);
}

/*
================================================================================
Function : CC1101WriteCmd( )
    Write a command byte to the device
INPUT    : command, the byte you want to write
OUTPUT   : None
================================================================================
*/
void CC1101WriteCmd( uint8_t command )
{   // uint8_t x1=0;
    CC_CSN_LOW( );
    // x1=SPI_ExchangeByte( command );
    SPI_ExchangeByte( command );
    CC_CSN_HIGH( );
    //printf("writeCmd -[%X]-read:[%X]\n",command,x1);
}

/*
================================================================================
Function : CC1101WriteReg( )
    Write a byte to the specified register
INPUT    : addr, The address of the register
           value, the byte you want to write
OUTPUT   : None
================================================================================
*/
void CC1101WriteReg( uint8_t addr, uint8_t value )
{
//		uint8_t x1=0,x2=0;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr );
    SPI_ExchangeByte( value );
    CC_CSN_HIGH( );
    //printf("write reg read data:[%X]-[%X]\n",x1,x2);

}

/*
================================================================================
Function : CC1101WORInit( )
    Initialize the WOR function of CC1101
INPUT    : None
OUTPUT   : None
================================================================================
*/
void  CC1101WORInit( void )
{
    CC1101WriteReg(CC1101_MCSM2,0x00);  //0x16
    CC1101WriteReg(CC1101_MCSM0,0x18);  //0X18
    CC1101WriteReg(CC1101_WOREVT1,0x8C);  //0x1E
    CC1101WriteReg(CC1101_WOREVT0,0xA0);  //0x1f
    CC1101WriteReg(CC1101_WORCTRL,0x78); //0X20   Wake On Radio Control

    CC1101WriteCmd( CC1101_SWORRST );
}

//------------自定义---------
void RF_RecvHandler_intrrupt(void)
{
    INT8U length=0, recv_buffer[64]= {0};
    uint8_t i=0;
    int RSSI_dBm=0;

    if(MY_CC1101_STATUS==0x01 || MY_CC1101_STATUS== 0x02) return;
    else MY_CC1101_STATUS=0X02;


    //__HAL_GPIO_EXTI_CLEAR_IT(PIN_CC_IRQ); //这个可以清除外部中断，每个中断的清除函数都不一样，需要分别调用
    //HAL_NVIC_EnableIRQ(EXIT_CC_IRQ_GD0);
    // if (1 == CC_IRQ_READ())             // 检测无线模块是否产生接收中断
    {
        while (CC_IRQ_READ() == 1);

        // 读取接收到的数据长度和数据内容
        length = CC1101RecPacket(recv_buffer);

        // 判断接收数据是否正确
        if (length <= SEND_MAX && length>0)
        {
            LED3_ON;               // LED闪烁，用于指示收到数据

            printf("RX-data:");
            for(i=0; i<length; i++)
            {
                Cmd_CC1101_RX[i]=recv_buffer[i];
                printf("[%X]--",recv_buffer[i]);
            }
            //printf("\n");
            // printf("%s\n",recv_buffer);

            RSSI_dBm=RS_buf_status[0];
            if(RSSI_dBm>=128) RSSI_dBm=(RSSI_dBm-256)/2-75;
            else RSSI_dBm=(RSSI_dBm)/2-75;

            printf("::RX_BUF_RSSI:[%X]-[%d]\n",RS_buf_status[0],RSSI_dBm);

            LED3_OFF;

        }
    }

    CC1101SetTRMode(RX_MODE);           // 进入接收模式
    MY_CC1101_STATUS=0X00;
    //CC1101WriteCmd(CC1101_SWOR); //设置WOR模式
}

void my_read_all_reg(void)
{
    int i=0;
    uint8_t read_buf[RF_reg_count]= {0};
    for(i=0; i<RF_reg_count; i++)
    {
        read_buf[i]=CC1101ReadReg( CC1101InitData[i][0]);

        // CC1101WriteReg( CC1101InitData[i][0], CC1101InitData[i][1] );

    }

    for(i=0; i<RF_reg_count; i++)
    {
        printf("address-[%2X] write-[%2X] read-[%2X]\n",CC1101InitData[i][0], CC1101InitData[i][1],read_buf[i]);

    }


}


/*
================================================================================
Function : CC1101SendPacket_add( )  带目标地址的发送函数
    Send a packet
INPUT    : txbuffer, The buffer stores data to be sent
           size, How many bytes should be sent
           mode, Broadcast or address check packet
OUTPUT   : None

参数address存放目标地址，一个字节
================================================================================
*/
void CC1101SendPacket_add( INT8U *txbuffer, INT8U size, TX_DATA_MODE mode,INT8U address)
{
    HAL_NVIC_DisableIRQ(EXIT_CC_IRQ_GD0); //关闭中断，不产生发射中断

    if( mode == BROADCAST )             {
        address = 0;
    }
    //else if( mode == ADDRESS_CHECK )    { address = CC1101ReadReg( CC1101_ADDR ); }

    CC1101ClrTXBuff( );

    //address=my_cc1101_dest_address; //测试使用，写入接收方地址
    if( ( CC1101ReadReg( CC1101_PKTCTRL1 ) & ~0x03 ) != 0 )
    {
        CC1101WriteReg( CC1101_TXFIFO, size + 1 ); //1个字节长度+上1个字节的地址+有效数据,任意长度方法
        CC1101WriteReg( CC1101_TXFIFO, address );  //地址+有效数据，固定长度方法
    }
    else
    {
        CC1101WriteReg( CC1101_TXFIFO, size ); //不加地址
    }

    CC1101WriteMultiReg( CC1101_TXFIFO, txbuffer, size ); //写入有效数据
    CC1101SetTRMode( TX_MODE );  //发送模式

    while( HAL_GPIO_ReadPin(PORT_CC_IRQ,PIN_CC_IRQ) != 1 );  //发送数据产生高电平中断
    while( HAL_GPIO_ReadPin(PORT_CC_IRQ,PIN_CC_IRQ) == 1 );  //高电平变为低电平

    CC1101ClrTXBuff( );  //情况发送FIFO
    CC1101SetTRMode(RX_MODE);  // 进入接收模式

    // __HAL_GPIO_EXTI_CLEAR_IT(PIN_CC_IRQ); //这个可以清除外部中断，每个中断的清除函数都不一样，需要分别调用
    //HAL_NVIC_EnableIRQ(EXIT_CC_IRQ_GD0); //---------开启接收中断

    //CC1101SetTRMode(RX_MODE);  // 进入接收模式


}
void RF_SendPacket3(void)  //测试使用，测试FIFO溢出的使用
{
    INT8U i=0,length=0, buffer[65]= {0};

    if ((0==COM_TimeOut) && (COM_RxCounter>0) )
    {
        if(MY_CC1101_STATUS==0x01 || MY_CC1101_STATUS== 0x02) return;
        else MY_CC1101_STATUS=0X01;

        //HAL_NVIC_DisableIRQ(EXIT_CC_IRQ_GD0); //关闭中断，不响应发射中断
        CC1101SetIdle();

        //LED3_ON;

        length = COM_RxCounter;

        COM_RxCounter = 0;

        for (i=0; i<length; i++)   {
            buffer[i] = COM_RxBuffer[i];
        }

        CC1101SendPacket(buffer, length, ADDRESS_CHECK);    // 发送数据，默认单地址发送方式




        // __HAL_GPIO_EXTI_CLEAR_IT(PIN_CC_IRQ); //这个可以清除外部中断，每个中断的清除函数都不一样，需要分别调用
        //	HAL_NVIC_EnableIRQ(EXIT_CC_IRQ_GD0);


        // CC1101SetTRMode(RX_MODE);                           // 进入接收模式

        printf("USART SINGLE TO CC1101\n");

        MY_CC1101_STATUS=0x00;
    }
}
