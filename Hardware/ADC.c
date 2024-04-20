#include "stm32f10x.h"

/*
1.非连续，非扫描获取角度
ADC2,ADC_Channel_6,1

2.扫描模式+连续模式+DMA转存获取电压，
利用Vrefint=1.2V作为模拟通道17
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
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC clock = PCLK2/6 APB2的时钟/6
	
	//GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//ADC 规则组通道和采样时间配置(ADC外设，ADC通道，在规则组中的次序，采样时间）
	ADC_RegularChannelConfig(ADC2,ADC_Channel_6,1,ADC_SampleTime_55Cycles5);

	ADC_RegularChannelConfig(ADC1,ADC_Channel_7,1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 2, ADC_SampleTime_55Cycles5);//ADC_Channel_17

	//ADC 初始化
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//工作模式：独立模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据对齐：右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部触发源：无（软件触发）
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 2;//规则组通道数目
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//连续转换模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 1;//规则组通道数目
	ADC_Init(ADC2,&ADC_InitStructure);
	
	//DMA初始化

	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//外设寄存器地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ;//每次转运的外设数据大小(8(字节),16(半字),32(字))[位]
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设寄存器地址是否自增   
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Value;										//内存地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ; //内存的数据大小
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//转运方向(以外设为源)
	DMA_InitStructure.DMA_BufferSize = 2;                        //传输计数器大小
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										//计数器是否重装(M2M不可自动重装)
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;											//是否软件触发
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//通道优先级
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);//只能选择DMA1_Channel1,ADC1的硬件触发只和该通道相连
	
	DMA_Cmd(DMA1_Channel1,ENABLE);//上电后仍需ADC1的硬件触发
	ADC_DMACmd(ADC1,ENABLE);//CR2_DMA 位置1，每完成规则组某通道的转换自动产生DMA请求
	//开启ADC，上电
	ADC_Cmd(ADC1,ENABLE);
	ADC_Cmd(ADC2,ENABLE);
	ADC_TempSensorVrefintCmd(ENABLE);
	
	//ADC校准
	ADC_ResetCalibration(ADC1);//重置校准：ADC1->CR2_RSTCAL_Set位置一，后被硬件自动清零
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);//等待重置校准完成，硬件自动清零
	ADC_StartCalibration(ADC1);//开始校准
	while(ADC_GetCalibrationStatus(ADC1)==SET);//等待校准完成，硬件自动清零
	
	
	ADC_ResetCalibration(ADC2);//重置校准：ADC1->CR2_RSTCAL_Set位置一，后被硬件自动清零
	while(ADC_GetResetCalibrationStatus(ADC2)==SET);//等待重置校准完成，硬件自动清零
	ADC_StartCalibration(ADC2);//开始校准
	while(ADC_GetCalibrationStatus(ADC2)==SET);//等待校准完成，硬件自动清零
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}






/*******************************Get_ADC_Angle获取倒立摆角度****************************************/

uint16_t Get_ADC_Angle(void){
	
	ADC_SoftwareStartConvCmd(ADC2,ENABLE);//软件触发ADC转换
	while(ADC_GetFlagStatus(ADC2,ADC_FLAG_EOC) == RESET);//等待转换结束（5.6us)，ADC->SR->EOC标志位置一
	return ADC_GetConversionValue(ADC2);//获取ADC->DR值，获取后EOC自动清零
	
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





/*******************************Get_ADC_Battery_Voltage获取电池电压****************************************/
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



