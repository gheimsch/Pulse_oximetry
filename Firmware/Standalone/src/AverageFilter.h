#ifndef __AVERAGE_FILTER_H_
#define __AVERAGE_FILTER_H_
/******************************************************************************/
/*! \file AverageFilter.h
******************************************************************************
* \brief Declaration for the average filter lib
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

#include <stdint.h>

/* ----------------------- module constant declaration -----------------------*/

#define NUMBER_OF_SAMPLES 	5

/* ------------------------- module type declaration -------------------------*/

typedef struct{

	uint32_t sum;
	uint16_t InputBuffer[NUMBER_OF_SAMPLES];
	uint8_t ActualIndex;
	uint16_t ValueThatGoesOut;
	uint16_t ValueThatGoesIn;
	uint16_t Result;
} MovingAverage_uint16_t;

typedef struct{

	uint16_t sum;
	uint8_t InputBuffer[NUMBER_OF_SAMPLES];
	uint8_t ActualIndex;
	uint8_t ValueThatGoesOut;
	uint8_t ValueThatGoesIn;
	uint8_t Result;
} MovingAverage_uint8_t;

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/

extern void MovingAverage_PushNewValue8b(MovingAverage_uint8_t *,uint8_t);
extern void MovingAverage_PushNewValue16b(MovingAverage_uint16_t *,uint16_t);
extern void MovingAverage_Clear8b(MovingAverage_uint8_t *);
extern void MovingAverage_Clear16b(MovingAverage_uint16_t *);

/* ****************************************************************************/
/* End Header : AverageFilter.h												  */
/* ****************************************************************************/
#endif /* __AVERAGE_FILTER_H_ */
