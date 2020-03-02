/**
  ******************************************************************************
  * @file    USART/USART_TwoBoards/DataExchangeInterrupt/main.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_DataExchangeInterrupt
  * @{
  */
/* �������� ---------------------------------------------------------*/
	//SDI
#define SDI_TX PA2
#define RS485_TX_EN PA11

uint8_t SDI_12_EN = 0;//Ĭ��SDI�˿ڴ�Ϊ����ģʽ,1���ͣ�0����
//���ջ����� 	
uint8_t SDI_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
uint8_t SDI_RX_LEN=0; 

	//��ʱ
static uint8_t  fac_us=0;//us��ʱ������
static uint16_t fac_ms=0;//ms��ʱ������


	//RS485
	
  uint8_t RS485_TX_EN = 0;//Ĭ��485�˿ڴ�Ϊ����ģʽ,1���ͣ�0����
  
  //���ջ����� 	
uint8_t RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
uint8_t RS485_RX_LEN=0;  

/* Private function prototypes -----------------------------------------------*/
static void init(void);
static void delay_init(uint16_t SYSCLK);

void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);
void USART2_IRQHandler(void);
void SDI_Receive_Data(uint8_t *buf,uint8_t *len);
void SDI_Send_signal(void);
void SDI_USART2_SendBuf(uint8_t *pBuf, uint8_t Len);
void SDI_CheckResponse(uint8_t *pBuf, uint8_t Len);
void USART1_IRQHandler(void);
void RS485_Receive_Data(uint8_t *buf,uint8_t *len);
void RS485_Send_Data(uint8_t *buf,uint8_t len);
/* Private functions ---------------------------------------------------------*/

int main(void)
{
	//��ʼ��
	init();//���ڣ��жϺ�IO��
	delay_init(8000000);//��ʱ
  
  /* Enable the USARTx Receive interrupt: this interrupt is generated when the
  USARTx receive data register is not empty */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//ʹ�ܽ����ж�
	

	while(1){
	//RS485����
	
	
	
	
	}
  
  
}




 /* ************��ʼ�� *******************/

static void init(void){
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//GPIOA��ʱ��ʹ��  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//����1ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//����2ʱ��ʹ��
  
  /* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);//���ô���2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);//���ô���1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
  
  
  /* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA9��PA10
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA2��PA3

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA11
  
  /* USARTx configuration ----------------------------------------------------*/
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);//����1��ʼ��
  
  USART_InitStructure.USART_BaudRate = 1200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);//����2��ʼ��
  
  /* NVIC configuration */
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);//����1�жϳ�ʼ��
  
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);//����2�жϳ�ʼ��
  
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);//ʹ�ܴ���
  USART_Cmd(USART2, ENABLE);
	
  
 
}

 /* ************SDI-12 *******************/


//����2���շ�����
void USART2_IRQHandler(void){
	
	uint8_t res;
	//�ж��Ƿ����ʹ��
	if(SDI_12_EN == 0){
	//�����ж��Ƿ��ǽ����ж�
	if(USART_GetITStatus(USART2,USART_IT_RXNE)){
		/*�жϽ��ջ������Ƿ�Ϊ�ǿ�*/
    while(!USART_GetFlagStatus(USART2,USART_FLAG_RXNE));
		res = USART_ReceiveData(USART2);//ȫ�ֱ�����ý���ֵ
		if(RS485_RX_LEN<64)
		{
			SDI_RX_BUF[RS485_RX_LEN] = res;		//��¼���յ���ֵ
			SDI_RX_LEN++;						//������������1 
		} 
	}
	}
	//�������64���ֽ���ô����
}

//SDI��ѯ�ͽ����ֵ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void SDI_Receive_Data(uint8_t *buf,uint8_t *len)
{
	uint8_t rxlen=RS485_RX_LEN;
	uint8_t i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==SDI_RX_LEN&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=SDI_RX_BUF[i];	
		}		
		*len=SDI_RX_LEN;	//��¼�������ݳ���
		RS485_RX_LEN=0;		//����
	}
}

void SDI_Send_signal(void){
	SDI_TX = 1;//�����жϿպ��źţ�����12ms
	dalay_ms(12);
	SDI_TX = 0;//�����жϴ����źţ�����8.33ms
	dalay_ms(8);
	dalay_us(330);
	SDI_12_EN = 1;//����ʹ��
}
//����2��������
void SDI_USART2_SendBuf(uint8_t *pBuf, uint8_t Len)
{
	//��������
    while(Len--)
    {
        /*�жϷ��ͻ������Ƿ�Ϊ��*/
        while(!USART_GetFlagStatus(USART2,USART_FLAG_TXE));
        USART_SendData(USART2,*pBuf++);
    }

	SDI_TX = 0;//�����������
	delay_ms(7);//��ֹͣλ��7.5ms�ڲ��ܷ����κ�����
	delay_us(500);
	SDI_12_EN = 0;//����ʹ��

}

	//����Ƿ���Ҫ�ط�
void SDI_CheckResponse(uint8_t *pBuf, uint8_t Len) {
	
	//��һ�֣��ڲ�����87msû���յ���Ӧ�������ͺ�����Ҫ��������������Ӧ����
	for(int i=0; i < 3; i++){
	Send_signal();//�����ж��ź�
	for(i=0; i < 3; i++){
	USART2_SendBuf(*pBuf, Len);//������Ϣ
	if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)){
	break;//���յ���Ϣ
	}
	delay_ms(25);//�ȴ�25ms�ٴ��ط�
	}
	if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)){
	break;
	}
	delay_ms(100);//�ȴ�100ms���ٴη����ж��ź��ط�
	}
	}
	

 /* ************RS485 *******************/
  
  

//����1���շ�����
void USART1_IRQHandler(void){

	uint8_t res;
	//�����ж��Ƿ��ǽ����ж�
	if(USART_GetITStatus(USART1,USART_IT_RXNE)){
	//�жϽ��ջ������Ƿ�Ϊ�ǿ�
    while(!USART_GetFlagStatus(USART1,USART_FLAG_RXNE));
		res = USART_ReceiveData(USART1);//��ʱ������ý���ֵ
		if(RS485_RX_LEN<64)
		{
			RS485_RX_BUF[RS485_RX_LEN] = res;		//��¼���յ���ֵ
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
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==RS485_RX_LEN&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_LEN;	//��¼�������ݳ���
		RS485_RX_LEN=0;		//����
	}
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(uint8_t *buf,uint8_t len)
{
	uint8_t t;
	RS485_TX_EN=1;			//����Ϊ����ģʽ
  	for(t=0;t<len;t++)		//ѭ����������
	{
	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);//�ȴ����ͽ���		
    USART_SendData(USART1,buf[t]); //��������
	}	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //�ȴ����ͽ���		  
	RS485_RX_TEN=0;
	RS485_TX_EN=0;				//����Ϊ����ģʽ	
}

	
	/* ************��ʱ���� *******************/


//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
static void delay_init(uint16_t SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//bit2���,ѡ���ⲿʱ��  HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}								    
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864 
void delay_ms(uint16_t nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL=0x01 ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  	    
}   
//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	 
}