/*
CC1101ͨ��ҵ���߼�����

*/

#include "my_cc_TX_RX.h"

#define COM_CC1101_WOR_WakeUp {0X68,0X05,0XFE,0X01,0XA0,0XA3,0XC1,0X16}
uint8_t MY_CC1101_STATUS=0;  //��ʶCC1101��ǰ״̬������Ĭ��00��TX Ϊ01��RXΪ02��WORΪ03
uint8_t MY_TX_RX_STEP=0;   //����Ĳ���



uint8_t Cmd_CC1101_TX[70]=COM_CC1101_WOR_WakeUp;
uint8_t Cmd_CC1101_RX[70]= {0};
uint16_t my_cmd_cc1101_step=0;


/*
���ܣ�����CC1101����
������
��һ����������洢����ַ��
�ڶ������������ֽ�����
�������������ŵ���
���ĸ�Ŀ��ĵ�ַ,
�����������ͬ����
*/

void my_cmd_cc1101_tx(uint8_t *pt_cmd,uint8_t cmd_size,uint8_t tx_channel,uint8_t des_address,uint16_t des_SYNC_word)
{
    uint8_t i=0;

    // HAL_NVIC_DisableIRQ(EXIT_CC_IRQ_GD0); //�ر��жϣ������������ж�
    CC1101SetIdle(); //�������ģʽ
    CC1101WriteReg(CC1101_CHANNR,tx_channel); //�任�ŵ�
    CC1101SetSYNC(des_SYNC_word); //����ͬ����


    //CC1101SendPacket_add( INT8U *txbuffer, INT8U size, TX_DATA_MODE mode,INT8U address)
    CC1101SendPacket_add(pt_cmd,cmd_size,ADDRESS_CHECK,des_address);  //��������

    //-----����ʹ��
    printf("TX-data:");
    for(i=0; i<cmd_size; i++)
    {
        printf("[%X]--",pt_cmd[i]);
    }
    printf("\n");

}





extern uint8_t my_CC1101_chip_address; //оƬ��ַ��Ĭ��Ϊ0x05
extern uint8_t my_cc1101_dest_address; //��������Ŀ���ַ
extern uint16_t my_CC1101_sync_word;  //CC1101ͬ���֣�Ĭ��Ϊ0x8799
extern uint8_t my_CC1101_change_channer;  //��̬�ŵ���



uint8_t my_temp=0;
//------����ʹ��-------------
int j=0;
void RF_SendPacket_cyc(void)
{
    INT8U i=0, length=0, buffer[65]= {0};



    // if ((0==COM_TimeOut) && (COM_RxCounter>0))
    {


        length = 4;

        //COM_RxCounter = 0;

        for (i=0; i<length; i++)
        {
            if(j>25) j=0;
            buffer[i] =j+0X41; //0x41
            j++;

        }
        buffer[i]='\n';
        // CC1101SendPacket(buffer, length+1, ADDRESS_CHECK);    // ��������
        CC1101SendPacket_add(buffer, length+1, ADDRESS_CHECK,my_cc1101_dest_address);
        //CC1101SetTRMode(RX_MODE);                           // �������ģʽ

        //DelayMs(250);


    }
}

/*
���ܣ�����WOR����֡
*/

void my_cmd_cc1101_tx_WakeCmd(void)
{

    uint8_t cmd_lenth=0;
    my_temp++;
    if(MY_CC1101_STATUS==0x01 || MY_CC1101_STATUS== 0x02) return;
    else MY_CC1101_STATUS=0X01;


    //0X68,0X05,0XFE,0X01,0XA0,0XA3,0XC1,0X16
    Cmd_CC1101_TX[0]=0X68;
    Cmd_CC1101_TX[1]=0X05;
    Cmd_CC1101_TX[2]=0XFE;
    Cmd_CC1101_TX[3]=0X01;
    Cmd_CC1101_TX[4]=0XA0;
    Cmd_CC1101_TX[5]=0XA3;
    Cmd_CC1101_TX[6]=my_temp;//0XC1;
    Cmd_CC1101_TX[7]=0X16;

//	my_CC1101_change_channer=0x02;
//	my_CC1101_sync_word=0x0102;
//	my_cc1101_dest_address=0xFE;
    cmd_lenth=8;

    //void my_cmd_cc1101_tx(uint8_t *pt_cmd,uint8_t cmd_size,uint8_t tx_channel,uint8_t des_address,uint16_t des_SYNC_word)
    my_cmd_cc1101_tx(Cmd_CC1101_TX,cmd_lenth,my_CC1101_change_channer,my_cc1101_dest_address,my_CC1101_sync_word);
    //printf("tx=%d\n",my_temp);


    MY_CC1101_STATUS=0x00;

}
/*
���ܣ�������������
*/

void my_cmd_cc1101_tx_cyc_data(void)
{

    uint8_t cmd_lenth=0;
    my_temp++;

    if(MY_CC1101_STATUS==0x01 || MY_CC1101_STATUS== 0x02) return;
    else MY_CC1101_STATUS=0X01;


    //0X68,0X05,0XFE,0X01,0XA0,0XA3,0XC1,0X16
    Cmd_CC1101_TX[0]=0X68;
    Cmd_CC1101_TX[1]=0X05;
    Cmd_CC1101_TX[2]=0XFE;
    Cmd_CC1101_TX[3]=0X01;
    Cmd_CC1101_TX[4]=0XA0;
    Cmd_CC1101_TX[5]=0XA3;
    Cmd_CC1101_TX[6]=my_temp;//0XC1;
    Cmd_CC1101_TX[7]=0X16;

//	my_CC1101_change_channer=0x02;
//	my_CC1101_sync_word=0x0102;
//	my_cc1101_dest_address=0xFE;
    cmd_lenth=8;

    //void my_cmd_cc1101_tx(uint8_t *pt_cmd,uint8_t cmd_size,uint8_t tx_channel,uint8_t des_address,uint16_t des_SYNC_word)
    my_cmd_cc1101_tx(Cmd_CC1101_TX,cmd_lenth,my_CC1101_change_channer,my_cc1101_dest_address,my_CC1101_sync_word);
    //printf("tx=%d\n",my_temp);


    MY_CC1101_STATUS=0x00;

}
/*
���ܣ�����ͻ��ң������
*/

void my_cmd_cc1101_tx_ALM_single_data(void)
{
    my_cmd_cc1101_tx_cyc_data();  //����ʹ�ã�û������*********
}

/*
����ͻ��ң������
*/
void my_cmd_cc1101_tx_ALM_multi_data(void)
{
    my_cmd_cc1101_tx_cyc_data();  //����ʹ�ã�û������*********

}
/*
�������ݷ��ͽ���
*/

void my_cmd_cc1101_tx_ALM_end_data(void)
{

    my_cmd_cc1101_tx_cyc_data();  //����ʹ�ã�û������*********
}

/*
���ܣ�����OK֡
*/
void my_cmd_cc1101_tx_OK_fram(void)
{


}

/*
���ܣ���Ӧ��ѯ���������Ͳ���
*/

void my_cmd_cc1101_tx_config_data(void)
{


}
/*
���ܣ���Ӧ��ѯ¼�����ݣ�����¼������
*/

void my_cmd_cc1101_tx_record_data(void)
{


}
/*
���ܣ���Ӧ��ѯ¼�����ݣ�����
*/

void my_cmd_cc1101_tx_record_end_data(void)
{


}







