#include "wdz_m35.h"
#include "string.h"
#include "cmsis_os.h"
#include "math.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;


extern uint8_t rsbuf1[];	  //USART1������
extern uint8_t rsbuf2[];   //USART2������
extern uint16_t rsbuf1pt_write;
extern uint16_t rsbuf1pt_read;
//extern uint16_t rsbuf1pt_TEMP_read;
extern uint16_t rsbuf2pt_write;
extern uint16_t rsbuf2pt_read;
//extern uint16_t rsbuf2pt_TEMP_read;

extern  uint32_t TimingDelay;
extern  uint8_t GPRS_Heartdata_error_count;

extern uint8_t AT_MESS_telphonenumber[7][16];
extern uint8_t AT_MESS_telphonenumber2[3][16];
extern uint16_t DTU_ADDRESS;
extern uint16_t Transmint_to_GPRS_hearttime;
extern uint16_t	Transmint_to_GPRS_Cycledata;


extern uint8_t MY_IP[4];
extern uint16_t MY_PORT;

uint8_t GPRS_Status=0;  //��ʶ�����ֻ�ģ�飬GPRS����״̬��1Ϊ�������������ӷ�������0Ϊ�����⣬��Ҫ����
uint8_t MESS_Status=0;  //��������״̬
uint8_t NET_Status=0;  //NET����״̬
uint8_t NET_Server_status=0; //Զ�˷�����server״̬



/*
����;M35��ʼ�����򣬽�������
*/
void my_init_m35()
{
    uint8_t *AT_COMM;
    uint8_t *AT_ANSWER;
    char IP0[4]= {0}; //�����洢10��ֹIP��ַ��ת���ɵ��ַ���������222��ת��32H,32H,32H
    char IP1[4]= {0};
    char IP2[4]= {0};
    char IP3[4]= {0};

    char PORT[7]= {0};

    char ip_port[45]= {0};

    u8 ii=0;
    uint8_t my_gprs_status=0;




    /*
    	//��һ�� ������ ������ʧ�ܺ����Ӳ���� ����������ʧ�ܿ���15���Ӷ�ʱ���˳�
        for(ii=0;ii<2;ii++)
    {
        AT_COMM=AT_REST;
    		AT_ANSWER=AN_Call_Ready;
    		my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,1,3000);
        if(my_gprs_status==0)
    		{
    			LED2_ON;
    			gprs_status=0;
    			//����Ӳ������
    			 Delay_us(1);

    		}
    	}
    	*/

    //��1������ѯ����״̬AT_CPIN,���������������ˣ�ֱ�Ӷϵ������ɡ����ϵ��������ʱ��15������
    for(ii=0; ii<2; ii++)
    {
        AT_COMM=AT_CPIN;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
        if(my_gprs_status==0)
        {
            //LED2_ON;
            GPRS_Status=0;
            //����Ӳ������
            //Delay_us(1);
        }
        else
        {
            break;
        }
    }

    //�ڶ��� ��AT��������
    if(my_gprs_status==1)
    {   AT_COMM=AT;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,10,1000);
    }



    //������  //���ò���
    if(my_gprs_status==1)
    {
        //Delay_us(100);
        AT_COMM=AT_V1;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,1,1000);//1����1��ָ��
        //Delay_us(100);

        AT_COMM=AT_E1;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,1,1000);
        //Delay_us(100);
        AT_COMM=AT_CEMEE;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,1,1000);
        //Delay_us(100);
        AT_COMM=AT_IPR;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,1,1000);
        //Delay_us(100);
        AT_COMM=AT_W;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,1,1000);
    }

    //��4����GPRS�������
    if(my_gprs_status==1)
    {
        AT_COMM=AT_CPIN;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }
    //--------���Ų���--------
    if(my_gprs_status==1)
    {
        AT_COMM=AT_CSMP;
        AT_ANSWER=AN_OK;
        //my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,500);
        my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,500);
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_CPMS_SM;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_CMGF;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_CSCS;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_CSDH;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);

        if(my_gprs_status==1)    //��������״̬��ʶ

        {   MESS_Status=1;
            //my_AT_MESS_dellall(); //���ԣ�ɾ�����ж���
        }
        else
            MESS_Status=0;
    }

    //----------���Ų��� ����--------

    //��6��������GPRS����
    //��4������ѯ����״̬//���ò���
    if(my_gprs_status==1)
    {
        AT_COMM=AT_CPIN;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_QIDEACT;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_CREG;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_QIMODE;
        AT_ANSWER=AN_OK;
        my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_QICSGP;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_QIREGAPP;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_QIACT;
        AT_ANSWER=AN_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
        //my_gprs_status=1;  //����ʹ�ã����ε���ָ���ֹͣ����
        if(my_gprs_status==1) NET_Status=1;  //����net����״̬��ʶ  AT+CSQ,�����ź�ǿ�� 13,0
        else NET_Status=0;
    }
    if(my_gprs_status==1)
    {
        AT_COMM=AT_QILOCIP;
        AT_ANSWER=AN_OK;
        //my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,1,100);  //��ѯIP��ַ�����Բ���
    }
    //��7��������TCP����
    if(my_gprs_status==1)
    {
        AT_COMM=AT_QICLOSE;

        AT_ANSWER=AN_CLOSE_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,1,1000);

        itoa(MY_IP[0], IP0, 10);
        itoa(MY_IP[1], IP1, 10);
        itoa(MY_IP[2], IP2, 10);
        itoa(MY_IP[3], IP3, 10);
        itoa(MY_PORT, PORT, 10);

        strcat(ip_port,"AT+QIOPEN=\"TCP\",");
        strcat(ip_port,"\"");
        strcat(ip_port,IP0);
        strcat(ip_port,".");
        strcat(ip_port,IP1);
        strcat(ip_port,".");
        strcat(ip_port,IP2);
        strcat(ip_port,".");
        strcat(ip_port,IP3);
        strcat(ip_port,"\",\"");
        strcat(ip_port,PORT);
        strcat(ip_port,"\"\r\n");
        AT_COMM=(uint8_t *)ip_port;

        AT_ANSWER=AN_CNNECT_OK;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,1000);
    }

    //���������ʧ�ܣ�����Ӳ����������15���Ӷ�ʱ��
    if(my_gprs_status==0)
    {
        LED1_ON; //��������һ����ɫ��,��ʾGPRS������
        GPRS_Status=0;
        NET_Server_status=0;
        // Delay_us(200);
        //����15���Ӷ�ʱ��15���Ӻ��ٴν��г�ʼ��
        my_speaker_long_voice(200);
    }
    else
    {
        GPRS_Status=1;
        NET_Server_status=0;
        GPRS_Heartdata_error_count=0;   //��������������
        LED1_OFF; //������һ����ƣ���������ʾ��������
        //����
        printf("====M35 IS OK!\n");
        my_speaker_short_cycle_voice(3,25);

    }

    //2016-5-28  �����ź�ǿ��
    AT_GET_CSQ();

    //2016-05-28 ����

}


/*

AT����Ļش������
������������ںţ��Ƚ��ַ���
�������������ش���ȷ����1���ش���󷵻�0��15�볬ʱ������0
*/
extern uint16_t my_tim6_count;
uint8_t my_AT_process_usart(uint8_t *Data,uint32_t delay_time)
{
    u32 address_first=USART_GPRS_BUFFERpt_READ;
    u32 i=0;
    char ch=0;
    uint8_t r_int=0;
    u16 j=0;
    uint16_t my_start_tim=0;

    my_start_tim=my_tim6_count;
    while((my_tim6_count-my_start_tim)<delay_time/100&&r_int==0)
    {   j=0;
        while(address_first!=USART_GPRS_BUFFERpt_WRITE)
        {
            ch=USART_GPRS_BUFFER[address_first];
            if(ch==Data[i])
            {   i++;
                address_first++;
            }
            else
            {   i=0;
                address_first++;
            }
            j++;
            if(address_first>=rsbuf_max)address_first=0;
            if(i>0&&Data[i]==0) {
                r_int=1;
                break;
            }
            else r_int=0;

        }
    }

    USART_GPRS_BUFFERpt_READ=address_first;  //���������ָ������ƶ����ƶ���֡��ĩβ

    return(r_int);

}

/*
���ܣ�ѭ������ATָ��n�Σ�ÿ���timeʱ�䣬�ɹ�����1��ʧ�ܷ���0
���룺�������ƣ�����at,Ӧ���ַ��������ʹ���������ʱ����TIME*10ms
������ɹ�1��ʧ��0
*/
uint8_t my_at_com(UART_HandleTypeDef* USARTx,uint8_t *AT_COMM,uint8_t *AT_ANSWER,uint8_t n,uint32_t time)
{
    u16 i=0;
    u8 comm_answer=0;
    uint8_t my_gprs_status=0;
    for(i=0; i<n; i++)
    {

        USART_printf(USARTx,AT_COMM);
        comm_answer=my_AT_process_usart(AT_ANSWER,time);  //1����1��ָ��
        if(comm_answer==1)
        {
            //USART_printf(&huart3,"%s=OK!!\r\n",AT_COMM);	  //����ʹ�ã���ʽʹ�ò�����
            my_gprs_status=1;
            break;
        }
        else
        {
            USART_printf(&huart3,"GPRS Send short data error: %s=%d\r\n",AT_COMM,i);
            my_gprs_status=0;

            USART_printf(USARTx,"\x1A");	//���ԣ�ǿ�ƽ������ζԻ�����
        }
    }
    return(my_gprs_status);
}





//***********����Ϣ��������*************




/*
  ����;��ȡ��ǰSIM���绰���룬��ȡ�ɹ�����1�����ɹ�����0���Ѷ�ȡ���ĵ绰������뵽������



AT+CPBS="ON"
[2014-12-23 09:31:59:323_S:] AT+CNUM
[2014-12-23 09:31:59:357_R:] AT+CNUM

[2014-12-23 09:31:59:404_R:] +CNUM: "","8618630674002",145,7,4

[2014-12-23 09:31:59:435_R:] OK

*/
uint8_t AT_Get_SIM_Telephonenumber(uint8_t *pt)
{
    uint8_t *AT_COMM;
    uint8_t *AT_ANSWER;
    uint8_t my_gprs_status=0;
    uint8_t firstaddress=0;
    uint8_t sencondaddress=0;
    uint8_t my_buf[256]= {0};
    uint16_t k=0;
    uint8_t my_status=0;

    AT_COMM=AT_CPBS;
    AT_ANSWER=AN_OK;
    my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,100);

    AT_COMM=AT_CNUM;
    my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,100);

    if(my_gprs_status==1)  //��ȡ�绰��
    {
        my_gprs_status=AT_Get_Frame(my_buf);

        if(my_gprs_status==1)
        {
            for(k=0; k<255; k++)
            {
                if(my_buf[k]==','&&my_status==0)
                {
                    firstaddress=k+2;
                    my_status=1;
                    k=k+2;

                }
                else if(my_buf[k]=='\"'&&my_status==1)
                {

                    sencondaddress=k;
                    my_status=2;
                    break;
                }

            }

            k=1;//��һ����Ԫ��������ַ�0x01����ʾ�ַ��������ŷ���ʱ�Զ���0X1A
            while(firstaddress<sencondaddress)
            {
                pt[k]=my_buf[firstaddress];
                firstaddress++;
                k++;
            }
        }
    }
    //�����ȡ�õĵ绰��Ϊ�պţ�����ûȡ�����ͷ���0��ȡ������1
    /*  ����ʹ��
    if(pt[0]!=pt[1])
    {
    	my_gprs_status=1;
    }
    else if(pt[0]==pt[1])
    {
    	my_gprs_status=0;
    }
    */
    return my_gprs_status;

}


/*
���ܣ���M35���صĽ����ȡһ֡���ݣ����뵽�����У�ԭ�������� �Ӻ�ǰ 0D 0A ֮��������ж�
����:�洢���ݵ������׵�ַ

[2014-12-23 09:31:59:323_S:] AT+CNUM
[2014-12-23 09:31:59:357_R:] AT+CNUM

[2014-12-23 09:31:59:404_R:] +CNUM: "","8618630674002",145,7,4

[2014-12-23 09:31:59:435_R:] OK
*/
uint8_t AT_Get_Frame(uint8_t *pt)
{
    uint16_t secondaddress=USART_GPRS_BUFFERpt_READ;
    uint16_t firstaddress=0;
    uint16_t k=0;
    uint8_t temp8=0;
    uint8_t my_status=0;

    if(secondaddress<8)secondaddress=USART_GPRS_BUFFERpt_READ+rsbuf_max-8;
    else secondaddress=USART_GPRS_BUFFERpt_READ-8;
    firstaddress=secondaddress;

    for(k=0; k<256; k++)
    {
        temp8=USART_GPRS_BUFFER[firstaddress];
        if(temp8==0X0A)
            my_status =1;
        else if(temp8==0X0D&&my_status==1)
            my_status=2;
        else my_status=0;

        if(my_status==2)
            break;

        if(firstaddress==0)
            firstaddress=rsbuf_max-1;
        else
            firstaddress=firstaddress-1;

    }

    //�����ݷ��뵽������
    if(my_status==2)
    {
        firstaddress=firstaddress+2;
        if(firstaddress>=rsbuf_max)
        {
            firstaddress=firstaddress-rsbuf_max;
        }
        k=0;

        while(firstaddress!=secondaddress)
        {
            pt[k]=USART_GPRS_BUFFER[firstaddress];
            k++;
            firstaddress++;
            if(firstaddress==rsbuf_max)
            {
                firstaddress=0;
            }
        }

        return 1;
    }
    else return 0;



}

/*
uint8_t GPRS_Status=0;  //��ʶ�����ֻ�ģ�飬GPRS����״̬��1Ϊ�������������ӷ�������0Ϊ�����⣬��Ҫ����
uint8_t MESS_Status=0;  //��������״̬
uint8_t NET_Status=0;  //NET����״̬
uint8_t NET_Server_status=0; //Զ�˷�����server״̬

���ܣ���飬����M35ģ�����������Ϊ3����Σ�
		���NET_Server_statusΪ0������Ϊ1��������M35�Ѿ�ע��GSM��GPRS���磬����Server���Ӳ��ϣ����³�ʼ��M35����������
		���NET_StatusΪ0����û��ע��GPRS���磬�������ӣ�������У����ػ�����������3�Σ����������
		��������У���M35�ϵ磬��ʼ��ʱ��2Сʱ�������ϵ�����
*/

//uint16_t my_M35_Reset_count=0;
extern uint8_t GPRS_Heartdata_error_count;
void AT_M35_Reset(void)
{


    //if(GPRS_Status==0 && NET_Status==1 && NET_Server_status==0) //M35ע����GPRS���磬����Serverû������  MESS_Status==1 &&
    if(GPRS_Heartdata_error_count<(5) && GPRS_Heartdata_error_count>=3)
    {
        my_init_m35();
        USART_printf(&huart3,"M35_rest-%d\n",GPRS_Heartdata_error_count);

    }
    //else if(GPRS_Status==0 && NET_Status==0 && NET_Server_status==0)  //M35û��ע��GPRS���磬Serverû������
    else if(GPRS_Heartdata_error_count>=(5) && GPRS_Heartdata_error_count<(10) )
    {

        M35_PWR_ON;      //�ߵ�ƽ����3�����ң��߹ػ�
        osDelay(3000);
        M35_PWR_OFF;    //�����͵�ƽ�������������߹ػ�״̬��Ϊ�͵�ƽ
        osDelay(100);
        //Delay_us(300);
        ////M35_PWR_ON;      //�ߵ�ƽ����3�����ң�����
        //Delay_us(100);
        //M35_PWR_OFF;    //�����͵�ƽ�������������߹ػ�״̬��Ϊ�͵�ƽ


        my_init_m35();  //��ʼ������
        //����ʹ��
        USART_printf(&huart3,"M35_PWR_CONTROL-%d\n",GPRS_Heartdata_error_count);
    }
    else if(GPRS_Heartdata_error_count>=(10) && GPRS_Heartdata_error_count<(15)) //GPRS_Heartdata_error_count%(12*5)<=2 &&
    {
//        M35_PWREN_CONTROL_OFF; //M35�ϵ�״̬
//        osDelay(1000);
//        M35_PWREN_CONTROL_ON;  //M35�ϵ磬��û�п���
//        osDelay(1000);

//        M35_PWR_ON;      //�ߵ�ƽ����3�����ң�����
//        osDelay(3000);
//        M35_PWR_OFF;    //�����͵�ƽ�������������߹ػ�״̬��Ϊ�͵�ƽ

        //=============
        M35_EMERG_OFF;
        M35_PWR_OFF;
        M35_PWREN_CONTROL_OFF; //M35�ϵ�״̬
        osDelay(1000);
        M35_PWREN_CONTROL_ON;  //M35�ϵ磬��û�п���
        M35_PWR_ON;      //�ߵ�ƽ����3�����ң��������߹ػ�
        osDelay(3000);
        M35_PWR_OFF;    //�����͵�ƽ�������������߹ػ�״̬��Ϊ�͵�ƽ


        //==============
        my_init_m35();  //��ʼ������
        //����ʹ��
        USART_printf(&huart3,"M35_PWREN_CONTROL%d\n",GPRS_Heartdata_error_count);
    }

    else if(GPRS_Heartdata_error_count>=(15))
    {
        //TIM3�ϵ磬���Ź��ϵ磬Ȼ��1����14�����ң��ϵ�������MCU

        //__HAL_RCC_TIM3_CLK_DISABLE();
        //����ʹ��
        USART_printf(&huart3,"M35_WATCHDOS_OFF-%d\n",GPRS_Heartdata_error_count);
        NVIC_SystemReset();
    }

}
/*
���ܣ����101Э���з����͵����ݳ���, �1460
*/
uint16_t get_string_number(uint8_t *string)
{   uint8_t *pt=string;
    uint16_t  re_number=0;
    uint16_t  re_number2=0;

    //���ֽڳ���
    if(*pt==0x10&&*(pt+5)==0x16)
    {
        re_number=6;
    }
    else if(*pt==0x68&&*(pt+3)==0x68)
    {
        re_number=*(pt+1)+6;
    }
    //˫�ֽڳ���
    else if(*pt==0x10&&*(pt+7)==0x16)
    {
        re_number=8;
    }
    else if(*pt==0x68&&*(pt+5)==0x68)
    {
        re_number=*(pt+2);
        re_number=(re_number<<8)+*(pt+1);

        re_number2=*(pt+4);
        re_number2=(re_number2<<8)+*(pt+3);

        if(re_number==re_number2)
            re_number=re_number+8;
        else
            re_number=0;
    }

    return re_number;
}


/*

���ܣ����ŷ��ͣ���DTU��������
*/

void my_reset_mcu2()  //����MCUͨ��������
{
    __disable_fault_irq();
    NVIC_SystemReset();
}


//���ܣ�--����AT+??ָ���÷��ؽ�����ַ�����������
//��������һ��������Ϊָ�,�ڶ���������Ϊ�ص�ָ���������Ϊ�洢����
uint8_t AT_Get_AT_Answer_string(uint8_t *AT_COMM,uint8_t *AT_ANSWER,uint8_t *pt)
{
    //uint8_t *AT_COMM;
    //uint8_t *AT_ANSWER;
    uint8_t my_gprs_status=0;

    uint8_t my_buf[256]= {0};
    uint16_t k=0;


    my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,100);

    if(my_gprs_status==1)
    {
        my_gprs_status=AT_Get_Frame(my_buf);

        if(my_gprs_status==1)
        {
            for(k=0; my_buf[k]!='\0' && k<256; k++)
            {
                pt[k]=my_buf[k];

            }
        }
    }

    return my_gprs_status;

}

/*
���ܣ�����AT+CSQָ������ź�ǿ��
*/

uint8_t MY_AT_CSQ_Value=0;  //�ź�ǿ�ȣ�0--31,99Ϊû���ź�

void AT_GET_CSQ()
{
    uint8_t *AT_COMM;
    uint8_t *AT_ANSWER;
    uint8_t my_buf[256]= {0};
//	uint8_t my_temp[256]={0};
    uint8_t result=0;
    uint8_t CSQ_Value=0;

    AT_COMM=AT_CSQ;
    AT_ANSWER=AN_OK;

    result=AT_Get_AT_Answer_string(AT_COMM,AT_ANSWER,my_buf);

    //HexToStr2(my_temp,my_buf,20);
    // USART_printf(&huart3,my_temp);
    // USART_printf(&huart3,"\r\n");

    if(result==1)
    {
        if(my_buf[7]==0X2C) //�ź�ǿ��ֻ��һλ��
        {
            CSQ_Value=my_buf[6]-0X30;
        }
        else            //�ź�ǿ����λ��
        {
            CSQ_Value=(my_buf[6]-0X30)*10+(my_buf[7]-0X30);
        }

        MY_AT_CSQ_Value=(uint8_t) abs((int)(CSQ_Value));


        //USART_printf(&huart3,"AT+CSQ=%s\r\n",my_buf);
        USART_printf(&huart3,"AT+CSQ=%d\r\n",MY_AT_CSQ_Value);

    }
    else
    {
        MY_AT_CSQ_Value=0XFF;
        USART_printf(&huart3,"AT+CSQ is ERROR!!\r\n");
    }

}

//=====2017-6-8===========

/*
���ܣ�AT�����TCP����
������������͵��ַ��������Ȳ�Ҫ����256�ֽ�==1460���ֽ�
������������ͳɹ�����1��ʧ�ܷ���0
*/
uint8_t my_at_senddata(uint8_t *string)
{
    uint8_t *AT_COMM;
    uint8_t *AT_ANSWER;
    uint8_t my_gprs_status=0;
    uint8_t transmit_buf[25]=AT_QISEND;
    uint8_t temp_number_buf[5]="0";
    int number=0;
//	char  ch=0x1A;
    if(GPRS_Status==1)
    {
        //���Ҫ���͵����ݵĳ����ֽ�����Ȼ���޸�AT_QISEND+����+\r\n������ȷ�����ȷ��ͷ�ʽ
        if(*string==0x10||*string==0x68)
        {
            number=get_string_number(string);
            temp_number_buf[0]=number/1000+0x30;
            temp_number_buf[1]=number%1000/100+0x30;
            temp_number_buf[2]=number%100/10+0x30;
            temp_number_buf[3]=number%10+0x30;

            strcat((char *)transmit_buf,(char *)temp_number_buf);
            strcat((char *)transmit_buf,"\r\n");
            AT_COMM=transmit_buf;  //�̶����ȷ���
        }
        else
        {
            AT_COMM=AT_QISEND2;	  //�ǹ̶����ȷ���1A����
        }



        AT_ANSWER=AN_QISEND_WAIT;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,300);
    }

    if(my_gprs_status==1)  //��������
    {

        AT_COMM=string;
        AT_ANSWER=AN_QISEND_OK;
        HAL_Delay(5);  //��ʱ��M35ת�����죬���ܽ�������
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,1,600);

    }
    return(my_gprs_status);


    //======
}

/*
���ܣ�ѭ������ATָ��n�Σ�ÿ���timeʱ�䣬�ɹ�����1��ʧ�ܷ���0
���룺�������ƣ�����at,Ӧ���ַ��������ʹ���������ʱ����TIME*10ms
������ɹ�1��ʧ��0
*/
uint8_t my_at_com_long_data(UART_HandleTypeDef* USARTx,uint8_t *AT_COMM,uint8_t *AT_ANSWER,uint8_t n,uint32_t time,uint16_t data_size)
{
    u16 i=0;
    u8 comm_answer=0;
    uint8_t my_gprs_status=0;
    for(i=0; i<n; i++)
    {

        //USART_printf(USARTx,AT_COMM);
        HAL_UART_Transmit(USARTx,AT_COMM,data_size,1000);
        comm_answer=my_AT_process_usart(AT_ANSWER,time);  //1����1��ָ��
        if(comm_answer==1)
        {
            //USART_printf(&huart3,"%s=OK!!\r\n",AT_COMM);	  //����ʹ�ã���ʽʹ�ò�����
            my_gprs_status=1;
            //GPRS_Heartdata_error_count=0;
            break;
        }
        else
        {
            USART_printf(&huart3,"GPRS Send long data error: %s=%d\r\n",AT_COMM,i);
            my_gprs_status=0;

            USART_printf(USARTx,"\x1A");	//���ԣ�ǿ�ƽ������ζԻ�����

            //GPRS_Heartdata_error_count++;//����ʧ�ܼ���������5�ξ�����M35@@@
        }
    }
    return(my_gprs_status);
}



//===========
#define my_GRPS_send_mx 1450
/*
���ܣ�����֡���ݷ��ͣ�1֡�1450���ֽڡ�
*/
uint8_t my_at_senddata_long(uint8_t *string)
{
    uint8_t *AT_COMM;
    uint8_t *AT_ANSWER;
    uint8_t my_gprs_status=0;
    uint8_t transmit_buf[25]=AT_QISEND;
    uint8_t temp_number_buf[5]="0";
    uint8_t my_block=0;
    uint16_t my_remain=0;
    uint8_t ii=0;
    int number=0;
//	char  ch=0x1A;
    if(GPRS_Status==1)
    {
        //���Ҫ���͵����ݵĳ����ֽ�����Ȼ���޸�AT_QISEND+����+\r\n������ȷ�����ȷ��ͷ�ʽ
        if(*string==0x10||*string==0x68)
        {
            number=get_string_number(string);
            if(number!=0)
                my_gprs_status=1;
            else
            {
                my_gprs_status=0;
                return my_gprs_status;
            }
        }
    }
    //=====
    if(my_gprs_status==1)
    {
        my_block=number/my_GRPS_send_mx;
        my_remain=number%my_GRPS_send_mx;

    }

//==========
    for(ii=0; ii<my_block && my_gprs_status==1 ; ii++)
    {
        temp_number_buf[0]=my_GRPS_send_mx/1000+0x30;
        temp_number_buf[1]=my_GRPS_send_mx%1000/100+0x30;
        temp_number_buf[2]=my_GRPS_send_mx%100/10+0x30;
        temp_number_buf[3]=my_GRPS_send_mx%10+0x30;

        //����sendָ��
        strcpy((char *)transmit_buf,AT_QISEND);
        strcat((char *)transmit_buf,(char *)temp_number_buf);
        strcat((char *)transmit_buf,"\r\n");
        AT_COMM=transmit_buf;  //�̶����ȷ���
        AT_ANSWER=AN_QISEND_WAIT;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,300);
        //��������
        if(my_gprs_status==1)  //��������
        {

            AT_COMM=string+ii*my_GRPS_send_mx;
            AT_ANSWER=AN_QISEND_OK;
            HAL_Delay(5);  //��ʱ��M35ת�����죬���ܽ�������
            my_gprs_status=my_at_com_long_data(USART_GPRS,AT_COMM,AT_ANSWER,1,600,my_GRPS_send_mx);

        }
    }
    if(my_remain!=0 && my_gprs_status==1)
    {

        temp_number_buf[0]=my_remain/1000+0x30;
        temp_number_buf[1]=my_remain%1000/100+0x30;
        temp_number_buf[2]=my_remain%100/10+0x30;
        temp_number_buf[3]=my_remain%10+0x30;

        //����sendָ��
        strcpy((char *)transmit_buf,AT_QISEND);
        strcat((char *)transmit_buf,(char *)temp_number_buf);
        strcat((char *)transmit_buf,"\r\n");
        AT_COMM=transmit_buf;  //�̶����ȷ���
        AT_ANSWER=AN_QISEND_WAIT;
        my_gprs_status=my_at_com(USART_GPRS,AT_COMM,AT_ANSWER,3,300);
        //��������
        if(my_gprs_status==1)  //��������
        {

            AT_COMM=string+my_block*my_GRPS_send_mx;
            AT_ANSWER=AN_QISEND_OK;
            HAL_Delay(5);  //��ʱ��M35ת�����죬���ܽ�������
            my_gprs_status=my_at_com_long_data(USART_GPRS,AT_COMM,AT_ANSWER,1,600,my_remain);

        }


    }

    return(my_gprs_status);


    //======
}
