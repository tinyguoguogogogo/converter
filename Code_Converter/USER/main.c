/* ͷ�ļ�------------------------------------------------------------------*/
#include "main.h"
/* �������� ---------------------------------------------------------*/

/*****SDI*************/
//������
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define _300BuadRate 3150
#define _600BuadRate 1700
#define _1200BuadRate 833
//����
#define COM_RX_STAT GPIO_ReadInputDataBit(COM_RX_PORT, COM_RX_PIN)
//����
u8 SDI_BUF[36];
u8  SDI_LEN = 0;
u8 recvData;
u32 delayTime;//1������Ҫ���ͺͽ��յ�ʱ��
#define COM_RX_PORT GPIOA
#define COM_RX_PIN GPIO_Pin_2
#define COM_TX_PORT GPIOA
#define COM_TX_PIN GPIO_Pin_2
#define COM_DATA_HIGH()	GPIO_SetBits(COM_TX_PORT, COM_TX_PIN) //�ߵ�ƽ
#define COM_DATA_LOW()	GPIO_ResetBits(COM_TX_PORT, COM_TX_PIN) //�͵�ƽ

uint8_t SDI_12_EN = 0;//Ĭ��SDI�˿ڴ�Ϊ����ģʽ,1���ͣ�0����
//���ջ����� 	
uint8_t SDI_RX_BUF[36];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
uint8_t SDI_RX_LEN=0; 
//���ձ�־λ
uint8_t SDI_RX_FLAG = 0;//1�����ݽ��յ��ˣ�0��û�н��յ�


	//��ʱ
static uint8_t  fac_us=0;//us��ʱ������
static uint16_t fac_ms=0;//ms��ʱ������

/*******RS485**********/
  //���ջ����� 	
uint8_t RS485_RX_BUF[36];  	//���ջ���,���36���ֽ�.
//���յ������ݳ���
uint8_t RS485_RX_LEN=0; 
uint8_t len = 0;

uint8_t Flag = 0;//��˫��ʹ�ܣ�flag=0��SDI��RS485��flag=1��RS485��SDI
uint8_t buf[36];//������
uint8_t buf_len = 0; //���յ����ݳ���

	uint8_t res1 = 0;
/* �������� -----------------------------------------------*/
void RS485_init(void);
void delay_init(void);

void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);
void USART2_IRQHandler(void);
void SDI_Receive_Data(uint8_t *buf,uint8_t *len);
void SDI_Send_signal(void);
void SDI_USART2_SendBuf(uint8_t *pBuf, uint8_t Len);
void SDI_CheckTimeOutAndSend(uint8_t *pBuf, uint8_t Len);
void USART1_IRQHandler(void);
void RS485_Receive_Data(uint8_t *buf,uint8_t *len);
void RS485_Send_Data(uint8_t *buf,uint8_t len);

void Change_TX(void);
void Change_RX(void);
void VirtualCOM_RX_GPIOConfig(void);
void VirtualCOM_TX_GPIOConfig(void);
void SDI_init(u16 baudRate);
void TIM3_Configuration(u16 period);
void VirtualCOM_ByteSend(u8 val);
void VirtualCOM_StringSend(u8 *str, u8 len);
void NOT(u8 *buf,u8 len);

/* ������ ---------------------------------------------------------*/

int main(void)
{	
	
	delay_init();
	RS485_init();
	SDI_init(_1200BuadRate);//���ò����ʺ�SDI���շ���IOģ�⴮�ڳ�ʼ��
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//ʹ�ܽ����ж�
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);//Ĭ��485�˿ڴ�Ϊ����ģʽ,1���ͣ�0����
while(1){

		if(Flag){  //����ʹ�ܱ�־
			//RS485���գ�SDI����
			RS485_Receive_Data(buf,&buf_len);
			if(buf_len != 0){
				SDI_CheckTimeOutAndSend(buf,buf_len);
			}
		} else{
			//SDI����,RS485����
			if(SDI_LEN != 0) {
			delay_ms(500);
			NOT(SDI_BUF,SDI_LEN);//ȡ��
			RS485_Send_Data(SDI_BUF, SDI_LEN);                                 
			}
		}
		}
	
}


 /* ************��ʼ�� *******************/
void RS485_init(void){
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*ʱ��*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//GPIOA��ʱ��ʹ��  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);//GPIOC��ʱ��ʹ��  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//����1ʱ��ʹ��

  /* ����IO��*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);//���ô���1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
  
  
  /* ��ʼ�����ÿ� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;//50MHZ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//����ģʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA9��PA10
	 
  /*��ʼ��IO��*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA3������SDI���������
	GPIO_ResetBits(GPIOA ,GPIO_Pin_3);//Ĭ�Ͻ��գ�0���գ�1����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA11������RS485�������
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
    GPIO_Init(GPIOC, &GPIO_InitStructure);//PA13��ʼ��IO�ڣ���ʾ���������־
	
  
  /* USARTx��ʼ�� ----------------------------------------------------*/
  USART_InitStructure.USART_BaudRate = 9600;//������
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//����Ҫ��żλ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���պͷ���
  USART_Init(USART1, &USART_InitStructure);//����1��ʼ��
  
  /* NVIC��ʼ��*/
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);//����1�жϳ�ʼ��
  
  
  /* ʹ�ܴ��� */
  USART_Cmd(USART1, ENABLE);//ʹ�ܴ���1
 
}



/********SDI��ʼ��**********/

//IO��ģ�⴮�ڽ���

//���ճ�ʼ��
void VirtualCOM_RX_GPIOConfig(void)

{

EXTI_InitTypeDef EXTI_InitStructure;

NVIC_InitTypeDef NVIC_InitStructure;

GPIO_InitTypeDef GPIO_InitStructure;
	

/* PA2Ϊ�������룬ģ��RX */
GPIO_InitStructure.GPIO_Pin = COM_RX_PIN;//PA2
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//����
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//������SDI12�Ǹ��߼���
GPIO_Init(COM_RX_PORT, &GPIO_InitStructure);//PA

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);//PA2ӳ�䵽�ж���2

EXTI_InitStructure.EXTI_Line=EXTI_Line2;	
EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising;//�������ж�
EXTI_InitStructure.EXTI_LineCmd=ENABLE;
EXTI_Init(&EXTI_InitStructure);


NVIC_InitStructure.NVIC_IRQChannel=EXTI2_3_IRQn; //�ⲿ�жϣ����ش���
NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
NVIC_Init(&NVIC_InitStructure);
EXTI_ClearITPendingBit(EXTI_Line2);//����жϱ�־

GPIO_ResetBits(GPIOA ,GPIO_Pin_3);//0�ǽ���

}
//���ͳ�ʼ��
void VirtualCOM_TX_GPIOConfig(void)
{

GPIO_InitTypeDef GPIO_InitStructure;

/* PA2��Ϊ��������ڣ�ģ��TX */
GPIO_InitStructure.GPIO_Pin = COM_TX_PIN;//PA2
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//����
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//������SDI12�Ǹ��߼�
GPIO_Init(COM_RX_PORT, &GPIO_InitStructure);//
GPIO_SetBits(COM_TX_PORT, COM_TX_PIN);
	

}
//SDI��˫��������
void Change_TX(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = COM_RX_PIN;//PA2
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//����
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//������SDI12�Ǹ��߼�
GPIO_Init(COM_TX_PORT, &GPIO_InitStructure);//PA
	
	GPIO_SetBits(GPIOA ,GPIO_Pin_3);//1�Ƿ���
	
	//�ر�SDI�ⲿ�ж�
	NVIC_InitStructure.NVIC_IRQChannel=EXTI2_3_IRQn; //�ⲿ�жϣ����ش���
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd=DISABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//SDI��˫��������
void Change_RX(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
GPIO_InitStructure.GPIO_Pin = COM_RX_PIN;//PA2
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//����
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//������SDI12�Ǹ��߼���
GPIO_Init(COM_RX_PORT, &GPIO_InitStructure);//PA
	
	GPIO_ResetBits(GPIOA ,GPIO_Pin_3);//0�ǽ���
	
	//�ⲿ�жϴ�
	NVIC_InitStructure.NVIC_IRQChannel=EXTI2_3_IRQn; //�ⲿ�жϣ����ش���
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line2);//����жϱ�־
}
//��ʱ����ʼ��
void TIM3_Configuration(u16 period)
{
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

NVIC_InitTypeDef NVIC_InitStructure;

RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ��TIM3��ʱ��


TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //Ԥ��Ƶϵ��Ϊ48������������ʱ��Ϊ48MHz/48 = 1MHz
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷ�Ƶ
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ü�����ģʽΪ���ϼ���ģʽ
TIM_TimeBaseStructure.TIM_Period = period - 1; //���ü��������С��ÿ��period�����Ͳ���һ�������¼�
TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);	//������Ӧ�õ�TIM3��

TIM_ClearFlag(TIM3, TIM_FLAG_Update);	//�������жϱ�־
TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����TIM3���ж�
TIM_Cmd(TIM3,DISABLE);	//�رն�ʱ��TIM3


NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	//ͨ������ΪTIM3�ж�
NVIC_InitStructure.NVIC_IRQChannelPriority = 1;//�ж����ȼ�1
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//���ж�
NVIC_Init(&NVIC_InitStructure);	
}


//���ò�����
void SDI_init(u16 baudRate)
{
u32 period;
VirtualCOM_TX_GPIOConfig();
VirtualCOM_RX_GPIOConfig();
if(baudRate == _300BuadRate)	//������300
period = _300BuadRate + 300;
else if (baudRate == _600BuadRate)	//������600
period = _600BuadRate + 300;
else if (baudRate == _1200BuadRate)	//������1200
period = _1200BuadRate + 50;//����
TIM3_Configuration(period);	//���ö�Ӧģ���ʵĶ�ʱ���Ķ�ʱʱ��
delayTime = baudRate;	//����IO���ڷ��͵�����
}
/********SDI���պͷ��ͺ���******************************/
enum{
COM_START_BIT,	//ֹͣλ
COM_D0_BIT,	//bit0
COM_D1_BIT,	//bit1
COM_D2_BIT,	//bit2
COM_D3_BIT,	//bit3
COM_D4_BIT,	//bit4
COM_D5_BIT,	//bit5
COM_D6_BIT,	//bit6
COM_D7_BIT,	//bit7
COM_STOP_BIT,	//bit8
};


u8 recvStat = COM_STOP_BIT;	//����״̬��

//�ⲿ�жϣ��ֽ���ʼλ��������ʱ�ж�
void EXTI2_3_IRQHandler(void)
{
	Flag=0;
	SDI_RX_FLAG = 1;//���յ����� 	
	if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
	{
	if(COM_RX_STAT) //������Ÿߵ͵�ƽ������Ǹߵ�ƽ����˵����⵽������
	{
		
	if(recvStat == COM_STOP_BIT)	//״̬Ϊֹͣλ
	{
	recvStat = COM_START_BIT;	//���յ���ʼλ
	TIM_Cmd(TIM3, ENABLE);	//�򿪶�ʱ������������
	}
	}
	EXTI_ClearITPendingBit(EXTI_Line2);	//���EXTI_Line2�жϹ����־λ	
	}
	
}


//��ʱ�жϣ���ʱ�����ֽڣ����߼�
void TIM3_IRQHandler(void)
{
if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //����Ƿ�����������¼�
{
TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);//����жϱ�־
recvStat++;	//�ı�״̬��
len++;
if(recvStat == COM_STOP_BIT) //�յ�ֹͣλ
{
	TIM_Cmd(TIM3, DISABLE);	//�رն�ʱ��
	SDI_BUF[SDI_LEN] = recvData;	
	SDI_LEN++;
	len=0;
	return; //������
}
if(len==8 && !COM_RX_STAT){
	recvData |= (1 << (recvStat - 1));//�̶��ڰ�λΪ0(���߼�Ϊ1)
	return;
}
if(COM_RX_STAT) //'1'�����߼���0
{
recvData |= (1 << (recvStat - 1));
}
else	//'0'���߼���1
{
recvData &= ~(1 <<(recvStat - 1));
}
}
}

//����һ���ֽڣ�ȡ������
void VirtualCOM_ByteSend(u8 val)
{
u8 i = 0;
GPIO_SetBits(COM_TX_PORT, COM_TX_PIN);	//��ʼλ
delay_us(delayTime);
for(i = 0; i < 8; i++)	//8λ����λ
{
if(val & 0x01)
GPIO_ResetBits(COM_TX_PORT, COM_TX_PIN);
else
GPIO_SetBits(COM_TX_PORT, COM_TX_PIN);
delay_us(delayTime);
val >>= 1;
}
GPIO_ResetBits(COM_TX_PORT, COM_TX_PIN);	//ֹͣλ
delay_us(delayTime);

}

//����һ���ַ���
void VirtualCOM_StringSend(u8 *str,u8 len)
{
	u8 t;
	for(t = 0; t < len; t++)
	{
		VirtualCOM_ByteSend(str[t]);
	}

}
//�����жϿպ��ź�
void SDI_Send_signal(void){
	GPIO_SetBits(COM_TX_PORT, COM_TX_PIN);//�����жϿպ��źţ�����12ms
	delay_ms(13);
	GPIO_ResetBits(COM_TX_PORT, COM_TX_PIN);//�����жϴ����źţ�����8.33ms
	delay_ms(8);
	delay_us(530);
}
	//����Ƿ���Ҫ�ط�,�ڲ�����87ms����Ҫ�����ж��źţ�֮����Ȼû���յ���Ӧ,100ms�󴫸��������˯��״̬
void SDI_CheckTimeOutAndSend(uint8_t *pBuf, uint8_t Len) {
	
	int i = 0;
	int k = 0;
	for(k=0; k < 3; k++){
	Change_TX();
	SDI_Send_signal();//�����ж��ź�
		for(i=0; i < 3; i++){
			Change_TX();
			VirtualCOM_StringSend(pBuf,Len);//PA2������Ϣ
			Change_RX();
			SDI_RX_FLAG = 0;
			delay_ms(25);//�ȴ�25ms���жϴ������Ƿ���Ӧ��������Ҫ�ط�
			if(SDI_RX_FLAG == 1){
				break;//���յ���Ϣ
				}
		}
		
		if(SDI_RX_FLAG == 1){
			break;
		}
		delay_ms(100);//�ȴ�100ms���ٴη����ж��ź��ط�
	}
	buf_len = 0;//RS485����������
	RS485_RX_LEN=0;	//����
	SDI_RX_FLAG = 0;//���ձ�־λ��0
}

//ȡ��
void NOT(u8 *buf,u8 len){
	int i=0;
	for(i=0;i<len;i++){
	buf[i] = ~buf[i];//ȡ��
	}
}

 /* ************RS485 *******************/
  
  

//����1���շ�����
void USART1_IRQHandler(void){
	uint8_t res1;
	Flag = 1;//��˫��ʹ��
	//�����ж��Ƿ��ǽ����ж�
	if(USART_GetITStatus(USART1,USART_IT_RXNE)){
	//�жϽ��ջ������Ƿ�Ϊ�ǿ�
    while(!USART_GetFlagStatus(USART1,USART_FLAG_RXNE));
		res1 = USART_ReceiveData(USART1);//��ʱ������ý���ֵ
		if(RS485_RX_LEN<36)
		{
			RS485_RX_BUF[RS485_RX_LEN] = res1;		//��¼���յ���ֵ
			RS485_RX_LEN++;						//������������1 
		}
	}

}

//RS485��ѯ�ͽ����ֵ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(uint8_t *buf,uint8_t *len)
{
	uint8_t rxlen=RS485_RX_LEN;
	uint8_t i=0;
	*len=0;				//Ĭ��Ϊ0��*len�Ƿ��㴫�Σ�����void���������޷����������ݳ�ȥ
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==RS485_RX_LEN&&rxlen)//���յ�������,�ҽ�������ˣ�==���ȼ��ߣ����ж��Ƿ�==�������Ƿ�Ϊ��
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_LEN;	//��¼�������ݳ���
	}
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(uint8_t *buf,uint8_t len)
{
	uint8_t t;
	GPIO_SetBits(GPIOA, GPIO_Pin_11);	//����Ϊ����ģʽ
  	for(t=0;t<len;t++)		//ѭ����������
	{
	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET)
	  {
	  };//�ȴ����ͽ���		
    USART_SendData(USART1,buf[t]); //��������
	}	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //�ȴ����ͽ���	
	SDI_LEN = 0; //SDI���ջ������	
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);//����Ϊ����ģʽ	
}

	
	/* ************��ʱ���� *******************/


//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SystemCoreClock:ϵͳʱ��
void delay_init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//systickʱ��= HCLK/8 
	fac_us = SystemCoreClock/8000000;
	fac_ms = fac_us*1000;
}								    
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864 
void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;
	
	SysTick->LOAD = nms*fac_ms;//ʱ�����
	SysTick->VAL = 0x00;        //���������
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;///��systick��ʱ������ʼ����ʱ
	
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01) && !(temp&(1<<16)));                                
	
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//�ر�systick��ʱ��
	SysTick->VAL = 0x00;//���������


}   
//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(uint32_t nus)
{		
uint32_t temp;
	
	SysTick->LOAD = nus*fac_us;                  //ʱ�����
	SysTick->VAL = 0x00;                        //���������

	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//��systick��ʱ������ʼ����ʱ
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01) && !(temp&(1<<16)));
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//�ر�systick��ʱ��
	SysTick->VAL = 0x00;//���������
}












	
