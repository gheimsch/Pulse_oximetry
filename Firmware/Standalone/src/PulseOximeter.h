#ifndef _PULSE_OXIMETER
#define _PULSE_OXIMETER

#include "AverageFilter.h"
#include "ADC.h"
#include "DAC.h"
#include <stdio.h>

//#define SPO2_DEBUG
#define SPO2_EXTERNAL_OPAMPS


/** ADC sampling period in ms */
#define SPO2_SAMPLING_PERIOD				1	//in ms

/** ADC channel for pressure sensor output */
#define	SPO2_ADC_CHANNEL_OUTPUT_SIGNAL		0	//ADC1 channel number

/** ADC channel for heart beat signal */
#define	SPO2_ADC_CHANNEL_BASELINE_SIGNAL		0


#define	SPO2_REAL_TIME_DATA_ARRAY_LENGTH		64		//bytes

#define SPO2_ARRAY_LENGTH			10 //8
#define OLDEST_ELEMENT				SPO2_ARRAY_LENGTH-1
#define NEWEST_ELEMENT				0

extern void SPO2Measuring(void);

extern void SPO2ResetVariables(void);

/** Call this function once before the main loop */
void SpO2_Init(void);

extern uint8_t SpO2_HeartRate;
extern uint8_t SpO2_SaturationValue;
extern uint8_t SpO2Graph[];
extern uint8_t SpO2_HeartBeatDetected;

uint32_t ElapsedTimeIn_us;

/** These are the possible events that the SpO2 can generate */
typedef enum
{
	SPO2_EVENT_NONE,
	SPO2_EVENT_MEASUREMENT_COMPLETE_OK,
	SPO2_EVENT_MEASUREMENT_ERROR,
	SPO2_EVENT_NEW_DATA_READY,
	SPO2_EVENT_DEBUG_MODE_NEW_DATA_READY,
} SpO2_Event_e;


/** Main SpO2 state machine states */
typedef enum
{
	SPO2_STATE_IDLE,
	SPO2_STATE_MEASURING
} SpO2States_e;


#endif //_PULSE_OXIMETER
