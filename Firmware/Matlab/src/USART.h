#ifndef __USART_H_
#define __USART_H_
/******************************************************************************/
/*! \file USART.h
******************************************************************************
* \brief Declaration for the USART interface
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

/* ----------------------- module constant declaration -----------------------*/

/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/

/*Initialize the USART interface*/
extern void InitUSART(void);
/*Send string over USART*/
extern void SendData(unsigned char);
/* ****************************************************************************/
/* End Header : USART.h														  */
/* ****************************************************************************/
#endif /* __USART_H_ */
