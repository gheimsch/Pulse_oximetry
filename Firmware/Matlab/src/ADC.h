#ifndef __ADC_H_
#define __ADC_H_
/******************************************************************************/
/*! \file ADC.h
******************************************************************************
* \brief Declaration for the Analog/Digital converter
*
*
*
* \author HEIG
*
* \version 0.0.1
*
* \history 02.01.2019 File Created
*
*/
/* ****************************************************************************/
/*Oximeter																	  */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_tim.h"
#include "I2C.h"

/* ----------------------- module constant declaration -----------------------*/
#define ADC_STEPS							65536
#define ADC_REF_VOLTAGE						8.192
#define ADC_STEP_SIZE						ADC_REF_VOLTAGE/ADC_STEPS

/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/

/*Initialize the ADC interface*/
extern void InitADS1115(void);
/*Get value from ADC*/
extern unsigned int getADCData(unsigned char);
/* ****************************************************************************/
/* End Header : ADC.h														  */
/* ****************************************************************************/
#endif /* __ADC_H_ */
