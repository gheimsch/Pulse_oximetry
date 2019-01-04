#ifndef __DAC_H_
#define __DAC_H_
/******************************************************************************/
/*! \file DAC.h
******************************************************************************
* \brief Declaration for the Digital/Analog converter
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
#include "stm32f4xx_dac.h"

/* ----------------------- module constant declaration -----------------------*/

/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/
/*Initialize the DAC interface*/
extern void InitDAC(void);
/*Switch red led on and set current*/
extern void SetRedLed(unsigned char);
/*Switch ir led on and set current*/
extern void SetIrLed(unsigned char);
/* ****************************************************************************/
/* End Header : DAC.h														  */
/* ****************************************************************************/
#endif /* __DAC_H_ */
