/******************************************************************************/
/*! \file DAC.c
******************************************************************************
* \brief File for the DAC interface
*
* Function: Interfaces the DAC to set the current from the red and ir LED
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

#include "DAC.h"		/* Include Headerfile */

/* ----------------------- module constant declaration -----------------------*/

/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/

/* ****************************************************************************/
/* End Header: DAC.c					   		  							  */
/* ****************************************************************************/

/******************************************************************************/
/* Function:  InitDAC		  												  */
/******************************************************************************/
/*! \brief Initialize the hardware for the DAC interface
*
* \author HEIG
*
* \version 0.0.1
*
* \date 02.01.2019 Function created
*
*******************************************************************************/
void InitDAC(void) {

    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;

    /* DAC Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /*enable outputs for the DAC*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*enable outputs for H-bridge PA2(red) and PA3(ir)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_2);
    GPIO_SetBits(GPIOA, GPIO_Pin_3);

	/* DAC channel 1(red) and 2(ir) Configuration */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	/* Enable DAC Channel 1 and 2 */
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);

}

/* ****************************************************************************/
/* End:  InitDAC				                                              */
/* ****************************************************************************/

/******************************************************************************/
/* Function:  SetRedLed		  												  */
/******************************************************************************/
/*! \brief Set the values for the red led
*
* \author HEIG
*
* \version 0.0.1
*
* \date 02.01.2019 Function created
*
*******************************************************************************/
void SetRedLed(unsigned char current){
	unsigned int i = 0;

	/*Switch off ir led*/
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
	DAC_SetChannel2Data(DAC_Align_12b_R, 0x000);

	/*Break before make*/
	for(i = 0; i < 10000; i++){
		;
	}

	/*Switch on red led*/
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);

	/*Set current*/
	if(current > 20){

		DAC_SetChannel1Data(DAC_Align_12b_R, 0xFFF);
	}
	else{

		DAC_SetChannel1Data(DAC_Align_12b_R, current * 204);
	}
}
/* ****************************************************************************/
/* End:  SetRedLed				                                              */
/* ****************************************************************************/

/******************************************************************************/
/* Function:  SetIrLed		  												  */
/******************************************************************************/
/*! \brief Set the values for the ir led
*
* \author HEIG
*
* \version 0.0.1
*
* \date 02.01.2019 Function created
*
*******************************************************************************/
void SetIrLed(unsigned char current){
	unsigned int i = 0;

	/*Switch off red led*/
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
	DAC_SetChannel1Data(DAC_Align_12b_R, 0x000);

	/*Break before make*/
	for(i = 0; i < 10000; i++){
		;
	}

	/*Switch on ir led*/
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);

	/*Set current*/
	if(current > 20){

		DAC_SetChannel2Data(DAC_Align_12b_R, 0xFFF);
	}
	else{

		DAC_SetChannel2Data(DAC_Align_12b_R, current * 204);
	}
}
/* ****************************************************************************/
/* End:  SetIrLed				                                              */
/* ****************************************************************************/
