/******************************************************************************/
/*! \file ADC.c
******************************************************************************
* \brief File for the ADC interface
*
* Function: Interfaces the ADS1115 to get the values from the red and ir LED
*
* Procedures:
*
* \author HEIG
*
* \version 0.0.1
*
* \date 02.01.2019 File Created
*
*/
/* ****************************************************************************/
/*Oximeter							                                          */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/

#include "ADC.h"	/* Include Headerfile */

/* ----------------------- module constant declaration -----------------------*/

/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/

uint16_t ADC1_Values[2]= {0};

/* ----------------------- module procedure declaration ----------------------*/

/* ****************************************************************************/
/* End Header: ADC.c					   		  							  */
/* ****************************************************************************/

/******************************************************************************/
/* Function:  InitADC		  												  */
/******************************************************************************/
/*! \brief Initialize the hardware for the ADC interface
*
* \author HEIG
*
* \version 0.0.1
*
* \date 02.01.2019 Function created
*
*******************************************************************************/
void InitADS1115(void){

	/*External 16bit ADC*/
	unsigned char data[2] = {0x83, 0xE3};

	/*Init I2C*/
	Init_I2C(I2C1, I2C_PinsPack_3, I2C_CLOCK_FAST_MODE);

	/*Write FS range 4.096 to adc*/
	I2C_WriteMulti(I2C1, 0x90, 0x01, data, 2);


	/*Internal 12bit ADC*/
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_CommonInitTypeDef ADC_CommonInitStruct;
    ADC_InitTypeDef ADC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_DMA2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /*enable inputs for the ADC*/
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*ADC1 configuration*/
    ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInit(&ADC_CommonInitStruct);

    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfConversion = 1;
    ADC_Init(ADC2, &ADC_InitStruct);

    /*Enable ADC channels*/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_15Cycles);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_7, 1, ADC_SampleTime_15Cycles);

    ADC_Cmd(ADC1, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    ADC_SoftwareStartConv(ADC1);
    ADC_SoftwareStartConv(ADC2);
}

/* ****************************************************************************/
/* End:  InitADC				                                              */
/* ****************************************************************************/

/******************************************************************************/
/* Function:  getADCData		  											  */
/******************************************************************************/
/*! \brief Get the data of the corresponding channel from ADS1115
*
* \author HEIG
*
* \version 0.0.1
*
* \date 02.01.2019 Function created
*
*******************************************************************************/
unsigned int getADCData(unsigned char channel){

	unsigned char conf[2] = {0x83, 0xE3};
	unsigned char adc_value[2];
	unsigned char conf_value[2] = {0x00, 0x00};
	unsigned int adc;

	 /*write channel to mux*/
	 if(channel == 0){

		 conf[0] = 0xC3;
	 }
	 if(channel == 1){

		 conf[0] = 0xD3;
		 while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET){
			 ;
		 }
		 adc = ADC_GetConversionValue(ADC1);
	 }
	 if(channel == 2){

		 conf[0] = 0xE3;
		 while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET){
			 ;
		 }
		 adc = ADC_GetConversionValue(ADC2);
	 }
	 if(channel == 3){

		 conf[0] = 0xF3;
	 }

//	/*Write mux value to config reg and trigger conversion*/
//	I2C_WriteMulti(I2C1, 0x90, 0x01, conf, 2);
//
//	/*Check if conversion is done*/
//	while((conf_value[0] & 0x80) != 128){
//
//		I2C_ReadMulti(I2C1, 0x90, 0x01, conf_value, 2);
//	}
//
//	I2C_ReadMulti(I2C1, 0x90, 0x00, adc_value, 2);
//
//	adc = adc_value[0] <<  8 | adc_value[1];

	return adc;
}
/* ****************************************************************************/
/* End:  getADCData				                                              */
/* ****************************************************************************/
