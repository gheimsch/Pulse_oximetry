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

	unsigned char data[2] = {0x83, 0x83};

	/*Init I2C*/
	Init_I2C(I2C1, I2C_PinsPack_3, I2C_CLOCK_FAST_MODE);

	/*Write FS range 4.096 to adc*/
	I2C_WriteMulti(I2C1, 0x90, 0x01, data, 2);

//    /*Timer to trigger the ADC conversion(100Hz)*/
//    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
//    TIM_TimeBaseStruct.TIM_Period = 84-1; // 100Hz
//    TIM_TimeBaseStruct.TIM_Prescaler = 10000-1; // 4200hz
//    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
//    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);
//    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
//    TIM_Cmd(TIM2, ENABLE);


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

	unsigned char conf[2] = {0x83, 0x83};
	unsigned char adc_value[2];
	unsigned char conf_value[2] = {0x00, 0x00};
	unsigned int adc;

	 /*write channel to mux*/
	 if(channel == 0){

		 conf[0] = 0xC3;
	 }
	 if(channel == 1){

		 conf[0] = 0xD3;
	 }
	 if(channel == 2){

		 conf[0] = 0xE3;
	 }
	 if(channel == 3){

		 conf[0] = 0xF3;
	 }

	/*Write mux value to config reg and trigger conversion*/
	I2C_WriteMulti(I2C1, 0x90, 0x01, conf, 2);

	/*Check if conversion is done*/
	while((conf_value[0] & 0x80) != 128){

		I2C_ReadMulti(I2C1, 0x90, 0x01, conf_value, 2);
	}

	I2C_ReadMulti(I2C1, 0x90, 0x00, adc_value, 2);

	adc = adc_value[0] <<  8 | adc_value[1];

	return adc;
}
/* ****************************************************************************/
/* End:  getADCData				                                              */
/* ****************************************************************************/


