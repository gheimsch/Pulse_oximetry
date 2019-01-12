/******************************************************************************/
/*! \file USART.c
******************************************************************************
* \brief File for the USART interface
*
* Function: Interfaces the USART
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

#include "USART.h"	/* Include Headerfile */

/* ----------------------- module constant declaration -----------------------*/

/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* ****************************************************************************/
/* End Header: ADC.c					   		  							  */
/* ****************************************************************************/

/******************************************************************************/
/* Function:  InitUSART		  												  */
/******************************************************************************/
/*! \brief Initialize the hardware for the USART interface
*
* \author HEIG
*
* \version 0.0.1
*
* \date 02.01.2019 Function created
*
*******************************************************************************/
void InitUSART(void){

	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3,&USART_InitStructure);

	/* Enable USART */
	USART_Cmd(USART3, ENABLE);

}

/* ****************************************************************************/
/* End:  InitUSART				                                              */
/* ****************************************************************************/

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART3, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
  {}

  return ch;
}


void sendString(const char *string){

	   while (*string)
	    {
	        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	        USART_SendData(USART3, *string++);
	    }
}
