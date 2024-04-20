#include "stm32f10x.h"

/*
1.����������ɨ���ȡ�Ƕ�
ADC2,ADC_Channel_6,1

2.ɨ��ģʽ+����ģʽ+DMAת���ȡ��ѹ��
����Vrefint=1.2V��Ϊģ��ͨ��17
ADC1,ADC_Channel_7,1
ADC1, ADC_Channel_Vrefint
*/
uint16_t ADC_Value[2];

void Adc_Init(void){
	
		//RCC
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC clock = PCLK2/6 APB2��ʱ��/6
	
	//GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//ADC ������ͨ���Ͳ���ʱ������(ADC���裬ADCͨ�����ڹ������еĴ��򣬲���ʱ�䣩
	ADC_RegularChannelConfig(ADC2,ADC_Channel_6,1,ADC_SampleTime_55Cycles5);

	ADC_RegularChannelConfig(ADC1,ADC_Channel_7,1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 2, ADC_SampleTime_55Cycles5);//ADC_Channel_17

	//ADC ��ʼ��
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ������ģʽ
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//���ݶ��룺�Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�ⲿ����Դ���ޣ����������
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����ת��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//ɨ��ģʽ
	ADC_InitStructure.ADC_NbrOfChannel = 2;//������ͨ����Ŀ
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//����ת��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//ɨ��ģʽ
	ADC_InitStructure.ADC_NbrOfChannel = 1;//������ͨ����Ŀ
	ADC_Init(ADC2,&ADC_InitStructure);
	
	//DMA��ʼ��

	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//����Ĵ�����ַ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ;//ÿ��ת�˵��������ݴ�С(8(�ֽ�),16(����),32(��))[λ]
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //����Ĵ�����ַ�Ƿ�����   
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Value;										//�ڴ��ַ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ; //�ڴ�����ݴ�С
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//ת�˷���(������ΪԴ)
	DMA_InitStructure.DMA_BufferSize = 2;                        //�����������С
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										//�������Ƿ���װ(M2M�����Զ���װ)
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;											//�Ƿ��������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//ͨ�����ȼ�
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);//ֻ��ѡ��DMA1_Channel1,ADC1��Ӳ������ֻ�͸�ͨ������
	
	DMA_Cmd(DMA1_Channel1,ENABLE);//�ϵ������ADC1��Ӳ������
	ADC_DMACmd(ADC1,ENABLE);//CR2_DMA λ��1��ÿ��ɹ�����ĳͨ����ת���Զ�����DMA����
	//����ADC���ϵ�
	ADC_Cmd(ADC1,ENABLE);
	ADC_Cmd(ADC2,ENABLE);
	ADC_TempSensorVrefintCmd(ENABLE);
	
	//ADCУ׼
	ADC_ResetCalibration(ADC1);//����У׼��ADC1->CR2_RSTCAL_Setλ��һ����Ӳ���Զ�����
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);//�ȴ�����У׼��ɣ�Ӳ���Զ�����
	ADC_StartCalibration(ADC1);//��ʼУ׼
	while(ADC_GetCalibrationStatus(ADC1)==SET);//�ȴ�У׼��ɣ�Ӳ���Զ�����
	
	
	ADC_ResetCalibration(ADC2);//����У׼��ADC1->CR2_RSTCAL_Setλ��һ����Ӳ���Զ�����
	while(ADC_GetResetCalibrationStatus(ADC2)==SET);//�ȴ�����У׼��ɣ�Ӳ���Զ�����
	ADC_StartCalibration(ADC2);//��ʼУ׼
	while(ADC_GetCalibrationStatus(ADC2)==SET);//�ȴ�У׼��ɣ�Ӳ���Զ�����
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}






/*******************************Get_ADC_Angle��ȡ�����ڽǶ�****************************************/

uint16_t Get_ADC_Angle(void){
	
	ADC_SoftwareStartConvCmd(ADC2,ENABLE);//�������ADCת��
	while(ADC_GetFlagStatus(ADC2,ADC_FLAG_EOC) == RESET);//�ȴ�ת��������5.6us)��ADC->SR->EOC��־λ��һ
	return ADC_GetConversionValue(ADC2);//��ȡADC->DRֵ����ȡ��EOC�Զ�����
	
}



#define MEAN_ANGLE_TIMES 10
uint16_t Get_Angle(void){
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<MEAN_ANGLE_TIMES;t++)
	{
		temp_val+=Get_ADC_Angle();
		//Delay_us(200);
	}
	return temp_val/MEAN_ANGLE_TIMES;

}





/*******************************Get_ADC_Battery_Voltage��ȡ��ص�ѹ****************************************/
#define FILTERING_TIMES 10
float Mean_Filter(float sensor)
{
	uint8_t i;
  float Sum_Speed = 0;     
 
  static  float Speed_Buf[FILTERING_TIMES]={0};
  for(i = 1 ; i<FILTERING_TIMES; i++)
  {
    Speed_Buf[i - 1] = Speed_Buf[i];
  }
  Speed_Buf[FILTERING_TIMES - 1] =sensor;

  for(i = 0 ; i < FILTERING_TIMES; i++)
  {
    Sum_Speed += Speed_Buf[i];
  }

	return Sum_Speed / FILTERING_TIMES;
}

float Get_Battery_Voltage(void){
	float Detected_Voltage=Mean_Filter(1.20*((float)ADC_Value[0]/ADC_Value[1]));
	float Battery_Voltage=Detected_Voltage*12.41/2.48;
	return Battery_Voltage;
	
	
}



