#include "PulseOximeter.h"

//public variables
uint8_t SpO2_HeartRate;
uint8_t SpO2_SaturationValue;
uint8_t SpO2_HeartBeatDetected;

/* Private functions */

static void CalculateHeartRateAndSpO2(void);

static void CalibrateIRLedIntensity(void);
static void CalibrateRedLedIntensity(void);

static void DoCalculations(void);
static void FindMaxAndMin(void);

typedef enum{

	SPO2_CONTROL_RED,
	SPO2_CONTROL_IR,
}SPO2_CONTROLS; //@TEST

typedef enum{

	SPO2_SUBSTATE_FINDING_MAX,
	SPO2_SUBSTATE_FINDING_MIN
}SPO2_SUBSTATES; //@TEST

/* Private variables */
static MovingAverage_uint16_t SpO2RedLedSignal;
static MovingAverage_uint16_t SpO2IRLedSignal;
static MovingAverage_uint16_t SpO2RedBaselineSignal;
static MovingAverage_uint16_t SpO2IRBaselineSignal;

static uint16_t SpO2IRMax = 0;
static uint16_t SpO2IRMin = 0;

static uint16_t SpO2RedMax = 0;
static uint16_t SpO2RedMin = 0;

static uint8_t SpO2ValidValue;

static uint8_t SpO2HeartRateArray[SPO2_ARRAY_LENGTH];
static uint8_t SpO2SpO2Array[SPO2_ARRAY_LENGTH];

uint8_t CopyOfSpO2HeartRateArray[SPO2_ARRAY_LENGTH];
uint8_t CopyOfSpO2SpO2Array[SPO2_ARRAY_LENGTH];

static uint16_t SpO2SamplesBetweenPulses = 0; //counts the samples between 2 max points to calculate the HR
static uint8_t SpO2ActualSubStateSubState = 0;
static uint8_t SpO2ActualEvent = SPO2_EVENT_NONE;
static uint8_t SpO2IsDiagnosticMode = 0;
static uint16_t TimerToRecalibrate = 0;

#define INITIAL_CURRENT      0

static uint16_t CurrentRedValue = INITIAL_CURRENT;
static uint16_t CurrentIRValue = INITIAL_CURRENT;

#define BASELINE_SETPOINT						1490	//1.2V
#define BASELINE_SETPOINT_UPPER_LIMIT_MAX		2234	//1.8V
#define BASELINE_SETPOINT_UPPER_LIMIT_MID		1862	//1.5V
#define BASELINE_SETPOINT_UPPER_LIMIT_MIN		1614	//1.3V

#define BASELINE_SETPOINT_LOWER_LIMIT_MAX		745		//0.6V
#define BASELINE_SETPOINT_LOWER_LIMIT_MID		1117	//0.9V
#define BASELINE_SETPOINT_LOWER_LIMIT_MIN		1365	//1.1V

#define CURRENT_MAX_VALUE	20.0

#define CURRENT_LARGE_STEP	5.0
#define CURRENT_MID_STEP	1.0
#define CURRENT_SHORT_STEP	0.1

#define IR_ON_TIME_us		50
#define IR_OFF_TIME_us		450

#define RED_ON_TIME_us		50
#define RED_OFF_TIME_us		450

#define RECALIBRATE_TIME	512

//the max freq we want to detect is 4Hz (250ms) (a person with tachicardia)
//we are sampling signal every 1ms so the comparation window must be 250 samples (250ms)

#define COMPARATION_WINDOW_INTERVAL		250			//samples between max and min


uint8_t maxCounter = 0;
uint8_t minCounter = 0;
uint16_t IRPeakToPeakVoltage = 0;
uint16_t RedPeakToPeakVoltage = 0;
uint16_t IrRms = 0;
uint16_t RedRms = 0;

const uint16_t Log10LookUpTable[] = { 0, 301, 602, 778, 903, 1000, 1079, 1146,
		1204, 1255, 1301, 1342, 1380, 1414, 1447, 1477, 1505, 1531, 1556, 1579,
		1602, 1623, 1643, 1662, 1681, 1698, 1716, 1732, 1748, 1763, 1778, 1792,
		1806, 1819, 1832, 1845, 1857, 1869, 1880, 1892, 1903, 1913, 1924, 1934,
		1944, 1954, 1963, 1973, 1982, 1991, 2000, 2008, 2017, 2025, 2033, 2041,
		2049, 2056, 2064, 2071, 2079, 2086, 2093, 2100, 2107, 2113, 2120, 2127,
		2133, 2139, 2146, 2152, 2158, 2164, 2170, 2176, 2181, 2187, 2193, 2198,
		2204, 2209, 2214, 2220, 2225, 2230, 2235, 2240, 2245, 2250, 2255, 2260,
		2264, 2269, 2274, 2278, 2283, 2287, 2292, 2296, 2301, 2305, 2309, 2313,
		2318, 2322, 2326, 2330, 2334, 2338, 2342, 2346, 2350, 2354, 2357, 2361,
		2365, 2369, 2372, 2376, 2380, 2383, 2387, 2390, 2394, 2397, 2401, 2404,
		2408, 2411, 2414, 2418, 2421, 2424, 2428, 2431, 2434, 2437, 2440, 2444,
		2447, 2450, 2453, 2456, 2459, 2462, 2465, 2468, 2471, 2474, 2477, 2480,
		2482, 2485, 2488, 2491, 2494, 2496, 2499, 2502, 2505, 2507, 2510, 2513,
		2515, 2518, 2521, 2523, 2526, 2528, 2531, 2534, 2536, 2539, 2541, 2544,
		2546, 2549, 2551, 2553, 2556, 2558, 2561, 2563, 2565, 2568, 2570, 2572,
		2575, 2577, 2579, 2582, 2584, 2586, 2588, 2591, 2593, 2595, 2597, 2599,
		2602, 2604, 2606, 2608, 2610, 2612, 2614, 2617, 2619, 2621, 2623, 2625,
		2627, 2629, 2631, 2633, 2635, 2637, 2639, 2641, 2643, 2645, 2647, 2649,
		2651, 2653, 2655, 2657, 2658, 2660, 2662, 2664, 2666, 2668, 2670, 2672,
		2673, 2675, 2677, 2679, 2681, 2683, 2684, 2686, 2688, 2690, 2691, 2693,
		2695, 2697, 2698, 2700, 2702, 2704, 2705, 2707, 2709, 2710, 2712, 2714,
		2716, 2717, 2719, 2720, 2722, 2724, 2725, 2727, 2729, 2730, 2732, 2733,
		2735, 2737, 2738, 2740, 2741, 2743, 2745, 2746, 2748, 2749, 2751, 2752,
		2754, 2755, 2757, 2758, 2760, 2761, 2763, 2764, 2766, 2767, 2769, 2770,
		2772, 2773, 2775, 2776, 2778, 2779, 2781, 2782, 2783, 2785, 2786, 2788,
		2789, 2790, 2792, 2793, 2795, 2796, 2797, 2799, 2800, 2802, 2803, 2804,
		2806, 2807, 2808, 2810, 2811, 2812, 2814, 2815, 2816, 2818, 2819, 2820,
		2822, 2823, 2824, 2826, 2827, 2828, 2829, 2831, 2832, 2833, 2835, 2836,
		2837, 2838, 2840, 2841, 2842, 2843, 2845, 2846, 2847, 2848, 2850, 2851,
		2852, 2853, 2854, 2856, 2857, 2858, 2859, 2860, 2862, 2863, 2864, 2865,
		2866, 2868, 2869, 2870, 2871, 2872, 2873, 2875, 2876, 2877, 2878, 2879,
		2880, 2881, 2883, 2884, 2885, 2886, 2887, 2888, 2889, 2890, 2892, 2893,
		2894, 2895, 2896, 2897, 2898, 2899, 2900, 2902, 2903, 2904, 2905, 2906,
		2907, 2908, 2909, 2910, 2911, 2912, 2913, 2914, 2915, 2916, 2918, 2919,
		2920, 2921, 2922, 2923, 2924, 2925, 2926, 2927, 2928, 2929, 2930, 2931,
		2932, 2933, 2934, 2935, 2936, 2937, 2938, 2939, 2940, 2941, 2942, 2943,
		2944, 2945, 2946, 2947, 2948, 2949, 2950, 2951, 2952, 2953, 2954, 2955,
		2956, 2957, 2958, 2959, 2959, 2960, 2961, 2962, 2963, 2964, 2965, 2966,
		2967, 2968, 2969, 2970, 2971, 2972, 2973, 2974, 2974, 2975, 2976, 2977,
		2978, 2979, 2980, 2981, 2982, 2983, 2984, 2984, 2985, 2986, 2987, 2988,
		2989, 2990, 2991, 2992, 2992, 2993, 2994, 2995, 2996, 2997, 2998, 2999,
		3000, 3000, 3001, 3002, 3003, 3004, 3005, 3006, 3006, 3007, 3008, 3009 };

/******************************************************************************
*                              State Measuring
******************************************************************************/

void SPO2Measuring(void){

	uint16_t poxOutputSignalRaw, SpO2BaselineSignalRaw;
	
	/* read SPO2 signals: these are the steps:
		1. turn on IR LED
		2. wait 
		3. read baseline and read IR signal
		4. turn off IR LED (both LEDs off)
		5. wait 
		6. turn on Red LED
		7. wait 
		8. read Red signal
		9. turn off Red LED (both LEDs off)
		10. wait
	*/
	
	uint16_t elapsedTimeCopy = ElapsedTimeIn_us;

	switch (SpO2ActualSubStateSubState){
		case 0:		
			SetIrLed(CurrentIRValue);	//start pwm with IR value
			
			SpO2ActualSubStateSubState++;
			break;
			
		case 1:
			if (elapsedTimeCopy >= IR_ON_TIME_us){

				ElapsedTimeIn_us = 0;

				poxOutputSignalRaw =   getADCData(2);	//read IR baseline and IR signal
				SpO2BaselineSignalRaw = getADCData(1);


				MovingAverage_PushNewValue16b(&SpO2IRBaselineSignal, SpO2BaselineSignalRaw);	//store IR baseline and IR signal
				MovingAverage_PushNewValue16b(&SpO2IRLedSignal, poxOutputSignalRaw);
				
				SetRedLed(0);
				SetIrLed(0);
				SpO2ActualSubStateSubState++;
			}
			break;
	
		case 2:
			if (elapsedTimeCopy >= IR_OFF_TIME_us){

				ElapsedTimeIn_us = 0;

				SetRedLed(CurrentRedValue);	//start pwm for Red LED
				SpO2ActualSubStateSubState++;					
			}
			break;
			
		case 3:
			if (elapsedTimeCopy >= RED_ON_TIME_us){

				ElapsedTimeIn_us = 0;

				poxOutputSignalRaw = getADCData(2);	//Read RED signal
				SpO2BaselineSignalRaw = getADCData(1);	//Read RED baseline

				MovingAverage_PushNewValue16b(&SpO2RedBaselineSignal, SpO2BaselineSignalRaw);	//store Red baseline and red signal
				MovingAverage_PushNewValue16b(&SpO2RedLedSignal, poxOutputSignalRaw);

				SetRedLed(0);
				SetIrLed(0);
		
				SpO2ActualSubStateSubState++;
			}
			break;
			
		case 4:
			if (elapsedTimeCopy >= RED_OFF_TIME_us){

				ElapsedTimeIn_us = 0;
				SpO2ActualSubStateSubState = 0;				
				SpO2SamplesBetweenPulses++;
									
				if (TimerToRecalibrate == 0){

					static unsigned char toogle = 0;
					
					#define MAX_CALIBRATION_ATTEMPTS	128
					static uint16_t calibrationAttempts = 0;
														
					//check if both baselines are within the limits					
					if 	((SpO2IRBaselineSignal.Result > BASELINE_SETPOINT_UPPER_LIMIT_MIN)  || (SpO2IRBaselineSignal.Result < BASELINE_SETPOINT_LOWER_LIMIT_MIN) ||
						((SpO2RedBaselineSignal.Result > BASELINE_SETPOINT_UPPER_LIMIT_MIN) || (SpO2RedBaselineSignal.Result < BASELINE_SETPOINT_LOWER_LIMIT_MIN))){

						if (toogle == SPO2_CONTROL_RED){

							if ((SpO2RedBaselineSignal.Result > BASELINE_SETPOINT_UPPER_LIMIT_MID) || (SpO2RedBaselineSignal.Result < BASELINE_SETPOINT_LOWER_LIMIT_MID)){

								CalibrateRedLedIntensity();						
								TimerToRecalibrate = 16;
								
								calibrationAttempts++;
							}
							toogle = SPO2_CONTROL_IR;							
						}
						else if (toogle == SPO2_CONTROL_IR){

							if ((SpO2IRBaselineSignal.Result > BASELINE_SETPOINT_UPPER_LIMIT_MID)  || (SpO2IRBaselineSignal.Result < BASELINE_SETPOINT_LOWER_LIMIT_MID)){

								CalibrateIRLedIntensity();
								TimerToRecalibrate = 16;

								calibrationAttempts++;
							}
							toogle = SPO2_CONTROL_RED;
						}
					}
					else{

						TimerToRecalibrate = RECALIBRATE_TIME;
						calibrationAttempts = 0;
					}
									
				}
				else{

					TimerToRecalibrate--;					
				}

				FindMaxAndMin();

				if ((SpO2ValidValue >= 10) && (!SpO2IsDiagnosticMode)){

					//generate pox ok event
					SpO2ActualEvent = SPO2_EVENT_MEASUREMENT_COMPLETE_OK;
					//SpO2_AbortMeasurement();
				}
			}
			break;
	}
}


/******************************************************************************
*                            Calibrate RED LED
******************************************************************************/
static void CalibrateRedLedIntensity(void){

	//if signal is larger than setpoint
	if (SpO2RedBaselineSignal.Result > BASELINE_SETPOINT_UPPER_LIMIT_MAX){

		if (CurrentRedValue > CURRENT_LARGE_STEP){

			//decrease offset by large steps
			CurrentRedValue -= CURRENT_LARGE_STEP;
		}
		else if (CurrentRedValue > CURRENT_MID_STEP){

			//decrease offset by large steps
			CurrentRedValue -= CURRENT_MID_STEP;
		}
		else if (CurrentRedValue > CURRENT_SHORT_STEP){

			//decrease offset by large steps
			CurrentRedValue -= CURRENT_SHORT_STEP;
		}
	}
	else if (SpO2RedBaselineSignal.Result > BASELINE_SETPOINT_UPPER_LIMIT_MID){

		//decrease offset by midium steps
		if (CurrentRedValue > CURRENT_MID_STEP){

			//decrease offset by large steps
			CurrentRedValue -= CURRENT_MID_STEP;
		}
		else if (CurrentRedValue > CURRENT_SHORT_STEP){

			//decrease offset by large steps
			CurrentRedValue -= CURRENT_SHORT_STEP;
		}		
	}
	else if (SpO2RedBaselineSignal.Result > BASELINE_SETPOINT_UPPER_LIMIT_MIN){

		//decrease offset by short steps		
		if (CurrentRedValue > CURRENT_SHORT_STEP){

			//decrease offset by large steps
			CurrentRedValue -= CURRENT_SHORT_STEP;
		}
	}
	
	
	//if signal is lower than setpoint	
	
	else if (SpO2RedBaselineSignal.Result < BASELINE_SETPOINT_LOWER_LIMIT_MAX){

		//increase offset by large steps
		if (CurrentRedValue < (CURRENT_MAX_VALUE - CURRENT_LARGE_STEP)){

			//decrease offset by large steps
			CurrentRedValue += CURRENT_LARGE_STEP;
		}	
		else if (CurrentRedValue < (CURRENT_MAX_VALUE - CURRENT_MID_STEP)){

			//decrease offset by large steps
			CurrentRedValue += CURRENT_MID_STEP;
		}
		else if (CurrentRedValue < (CURRENT_MAX_VALUE - CURRENT_SHORT_STEP)){

			//decrease offset by large steps
			CurrentRedValue += CURRENT_SHORT_STEP;
		}
		//else CurrentRedValue = 0;
		
	}
	else if (SpO2RedBaselineSignal.Result < BASELINE_SETPOINT_LOWER_LIMIT_MID){

		//increase offset by medium steps
		if (CurrentRedValue < (CURRENT_MAX_VALUE - CURRENT_MID_STEP)){

			//decrease offset by large steps
			CurrentRedValue += CURRENT_MID_STEP;
		}	
		else if (CurrentRedValue < (CURRENT_MAX_VALUE - CURRENT_SHORT_STEP)){

			//decrease offset by large steps
			CurrentRedValue += CURRENT_SHORT_STEP;
		}	
	}
	else if (SpO2RedBaselineSignal.Result < BASELINE_SETPOINT_LOWER_LIMIT_MIN){

		//increase offset by short steps		
		if (CurrentRedValue < (CURRENT_MAX_VALUE - CURRENT_SHORT_STEP)){

			//decrease offset by large steps
			CurrentRedValue += CURRENT_SHORT_STEP;
		}
	}	
}

/******************************************************************************
*                              Calibrate IR LED
******************************************************************************/
static void CalibrateIRLedIntensity(void){

	//if signal is larger than setpoint
	if (SpO2IRBaselineSignal.Result > BASELINE_SETPOINT_UPPER_LIMIT_MAX){

		if (CurrentIRValue > CURRENT_LARGE_STEP){

			//decrease offset by large steps
			CurrentIRValue -= CURRENT_LARGE_STEP;
		}
		else if (CurrentIRValue > CURRENT_MID_STEP){

			//decrease offset by large steps
			CurrentIRValue -= CURRENT_MID_STEP;
		}
		else if (CurrentIRValue > CURRENT_SHORT_STEP){

			//decrease offset by large steps
			CurrentIRValue -= CURRENT_SHORT_STEP;
		}
		//else CurrentIRValue = CURRENT_MAX_VALUE;
	}
	else if (SpO2IRBaselineSignal.Result > BASELINE_SETPOINT_UPPER_LIMIT_MID){

		//decrease offset by midium steps
		if (CurrentIRValue > CURRENT_MID_STEP){

			//decrease offset by large steps
			CurrentIRValue -= CURRENT_MID_STEP;
		}
		else if (CurrentIRValue > CURRENT_SHORT_STEP){

			//decrease offset by large steps
			CurrentIRValue -= CURRENT_SHORT_STEP;
		}		
	}
	else if (SpO2IRBaselineSignal.Result > BASELINE_SETPOINT_UPPER_LIMIT_MIN){

		//decrease offset by short steps		
		if (CurrentIRValue > CURRENT_SHORT_STEP){

			//decrease offset by large steps
			CurrentIRValue -= CURRENT_SHORT_STEP;
		}
	}
	
	
	//if signal is lower than setpoint	
	
	else if (SpO2IRBaselineSignal.Result < BASELINE_SETPOINT_LOWER_LIMIT_MAX){

		//increase offset by large steps
		if (CurrentIRValue < (CURRENT_MAX_VALUE - CURRENT_LARGE_STEP)){

			//decrease offset by large steps
			CurrentIRValue += CURRENT_LARGE_STEP;
		}
		else if (CurrentIRValue < (CURRENT_MAX_VALUE - CURRENT_MID_STEP)){

			//decrease offset by large steps
			CurrentIRValue += CURRENT_MID_STEP;
		}
		else if (CurrentIRValue < (CURRENT_MAX_VALUE - CURRENT_SHORT_STEP)){

			//decrease offset by large steps
			CurrentIRValue += CURRENT_SHORT_STEP;
		}
	}
	else if (SpO2IRBaselineSignal.Result < BASELINE_SETPOINT_LOWER_LIMIT_MID){

		//increase offset by medium steps
		if (CurrentIRValue < (CURRENT_MAX_VALUE - CURRENT_MID_STEP)){

			//decrease offset by large steps
			CurrentIRValue += CURRENT_MID_STEP;
		}
		else if (CurrentIRValue < (CURRENT_MAX_VALUE - CURRENT_SHORT_STEP)){

			//decrease offset by large steps
			CurrentIRValue += CURRENT_SHORT_STEP;
		}	
	}
	else if (SpO2IRBaselineSignal.Result < BASELINE_SETPOINT_LOWER_LIMIT_MIN){

		//increase offset by short steps		
		if (CurrentIRValue < (CURRENT_MAX_VALUE - CURRENT_SHORT_STEP)){

			//decrease offset by large steps
			CurrentIRValue += CURRENT_SHORT_STEP;
		}
	}		
}

/******************************************************************************
*                             Find MAX and MIN
******************************************************************************/


static void FindMaxAndMin(void){

	static uint16_t comparationWindowTimer;
	static uint8_t subStateFindingMaxAndMin;
		
	
	if (subStateFindingMaxAndMin == SPO2_SUBSTATE_FINDING_MAX){

		//if the SpO2 signal is going up
		if (SpO2IRLedSignal.Result > SpO2IRMax){

			SpO2IRMax = SpO2IRLedSignal.Result;							//store new max values
			SpO2RedMax = SpO2RedLedSignal.Result;
			
			comparationWindowTimer = COMPARATION_WINDOW_INTERVAL;		//restart counter				
		}
		else{

			//slope changed, now signal is going down
			comparationWindowTimer--;
			
			if (comparationWindowTimer == 0){

				//no more max peaks detected in the window time, go to detection of min
				subStateFindingMaxAndMin = SPO2_SUBSTATE_FINDING_MIN;
				maxCounter++;
				SpO2IRMin = SpO2IRMax;
				SpO2RedMin = SpO2RedMax;
			}
		}
	}
	else if (subStateFindingMaxAndMin == SPO2_SUBSTATE_FINDING_MIN){

		//if the SpO2 signal is going down
		if (SpO2IRLedSignal.Result < SpO2IRMin){

			SpO2IRMin = SpO2IRLedSignal.Result;							//store new min values
			SpO2RedMin = SpO2RedLedSignal.Result;
			comparationWindowTimer = COMPARATION_WINDOW_INTERVAL;		//restart counter
		}
		else{

			//slope changed, now signal is going up
			comparationWindowTimer--;
			
			if (comparationWindowTimer == 0){

				//no more min peaks detected in the window time, go to next state
				subStateFindingMaxAndMin = SPO2_SUBSTATE_FINDING_MAX;					
				minCounter++;

				DoCalculations();		//do calculations before updating min and max values
				
				SpO2IRMax = SpO2IRMin;
				SpO2RedMax = SpO2RedMin;
				SpO2_HeartBeatDetected = 1;
			}
		}
	}
}

/******************************************************************************
*                               Do Calculations
******************************************************************************/

static void DoCalculations(void){

	uint8_t poxInstantaneousHeartRate;
	uint8_t poxInstantaneousSpO2;
	uint8_t i=0;
	
	uint32_t numerator;
	uint16_t lookUpTableIndex = 0;

	IRPeakToPeakVoltage = SpO2IRMax - SpO2IRMin;		//calculate peak to peak voltage
	RedPeakToPeakVoltage = SpO2RedMax - SpO2RedMin;
	
	if (IRPeakToPeakVoltage < 2000 && RedPeakToPeakVoltage < 2000 && IRPeakToPeakVoltage > 20 && RedPeakToPeakVoltage >20){

		//calculate Vrms = 0.5*Vpp/sqrt(2)
		numerator = (uint32_t)((uint32_t)IRPeakToPeakVoltage * 707);
		IrRms = (uint16_t)((uint32_t)(numerator)/(uint32_t)(2*1000));
		
		numerator = (uint32_t)((uint32_t)RedPeakToPeakVoltage * 707);
		RedRms = (uint16_t)((uint32_t)(numerator)/(uint32_t)(2*1000));
		
		if (RedRms < 1024 && IrRms < 1024){
	
            lookUpTableIndex = (uint16_t)(RedRms/2);
			numerator = (uint32_t)((uint32_t)Log10LookUpTable[lookUpTableIndex]*(uint32_t)100);
			
			lookUpTableIndex = (uint16_t)(IrRms/2);

			poxInstantaneousSpO2 = (uint8_t) ((uint32_t)(numerator)/(uint32_t)Log10LookUpTable[lookUpTableIndex]);
                  
			if (poxInstantaneousSpO2 > 100){

				poxInstantaneousSpO2 = 100;
			}
			
			//Calculate instantaneous HR //60000
			poxInstantaneousHeartRate = (uint8_t) (60000 / (SpO2SamplesBetweenPulses));
			
			//Shift samples of FIFO	
			for (i=OLDEST_ELEMENT; i>NEWEST_ELEMENT; i--){

				SpO2HeartRateArray[i] = SpO2HeartRateArray[i-1];	
				SpO2SpO2Array[i] = SpO2SpO2Array[i-1];
			}
			
			//insert new sample into FIFO
			SpO2HeartRateArray[NEWEST_ELEMENT] = poxInstantaneousHeartRate;
			SpO2SpO2Array[NEWEST_ELEMENT] = poxInstantaneousSpO2;
					
			CalculateHeartRateAndSpO2();		//calculate average of medians
			
			SpO2SamplesBetweenPulses = 0;		//Restart samples counter 									
		}
                
		else{

			SpO2_HeartRate = 0;
			SpO2_SaturationValue = 0;
		}
	}
	else{

		SpO2_HeartRate = 0;
		SpO2_SaturationValue = 0;
		printf("HeartRate:%dHz\n",0);
		printf("SPO2:%d%%\n",0);
	}
}

/******************************************************************************
*                          Calculate HR and SpO2
******************************************************************************/

static void CalculateHeartRateAndSpO2(void){
//order HearRate values in array and average samples in the middle		

	uint8_t startIndex;
	uint8_t smallestIndex;
	uint8_t currentIndex;
	uint8_t tempStoreValue;
	uint16_t sum;
	uint8_t i;
	
	//Create a copy of the arrays
	for (i=0; i<SPO2_ARRAY_LENGTH; i++){

		CopyOfSpO2HeartRateArray[i] = SpO2HeartRateArray[i];		
		CopyOfSpO2SpO2Array[i] = SpO2SpO2Array[i];
	}
	
	// Order array values in ascending order 
	for (startIndex = 0; startIndex < SPO2_ARRAY_LENGTH; startIndex++){

		smallestIndex = startIndex;

		for (currentIndex = startIndex + 1; currentIndex < SPO2_ARRAY_LENGTH; currentIndex++){

			if (CopyOfSpO2HeartRateArray[currentIndex] < CopyOfSpO2HeartRateArray[smallestIndex]){

				smallestIndex = currentIndex;
			}
		}

		tempStoreValue = (uint8_t) CopyOfSpO2HeartRateArray[startIndex];

		CopyOfSpO2HeartRateArray[startIndex] = CopyOfSpO2HeartRateArray[smallestIndex];
		CopyOfSpO2HeartRateArray[smallestIndex] = tempStoreValue;
	}						
	
	// Calculate HR averaging values from 2-5 (avoids using false HR values)
	//
	//	 		 	                  		  [0,1,2,3,4,5,6,7] 
	//										   | |         | |
	//										   | |         | |
	//									remove - -         - -
	
	sum = 0;
	
	for (i = 2; i < 6; i++){

		sum += CopyOfSpO2HeartRateArray[i];
	}

	SpO2_HeartRate = (uint8_t)(sum / 4);
	
	
	//Same procedure, but with SpO2 array
	
	
	// Order array values in ascending order 
	for (startIndex = 0; startIndex < SPO2_ARRAY_LENGTH; startIndex++){

		smallestIndex = startIndex;

		for (currentIndex = startIndex + 1; currentIndex < SPO2_ARRAY_LENGTH; currentIndex++){

			if (CopyOfSpO2SpO2Array[currentIndex] < CopyOfSpO2SpO2Array[smallestIndex]){

				smallestIndex = currentIndex;
			}
		}

		tempStoreValue = (uint8_t) CopyOfSpO2SpO2Array[startIndex];

		CopyOfSpO2SpO2Array[startIndex] = CopyOfSpO2SpO2Array[smallestIndex];
		CopyOfSpO2SpO2Array[smallestIndex] = tempStoreValue;
	}						
	
	// Calculate SpO2 averaging values from 2-5 (avoids using false values)
	//
	//	 		 	                  		  [0,1,2,3,4,5,6,7] 
	//										   | |         | |
	//										   | |         | |
	//									remove - -         - -
	
	sum = 0;
	
	for (i = 2; i < 6; i++){

		sum += CopyOfSpO2SpO2Array[i];
	}

	SpO2_SaturationValue = (uint8_t)(sum / 4);
	
	printf("HeartRate:%dHz\n",SpO2_HeartRate);
	printf("SPO2:%d%%\n",SpO2_SaturationValue);

	SpO2ValidValue++;
}

/******************************************************************************
*                                 Reset Variables
******************************************************************************/

void SPO2ResetVariables(void){

	uint8_t i = 0;
	
	//Reset variables
	SpO2IRMax = 0;
	SpO2IRMin = 4096;
	
	SpO2RedMax = 0;
	SpO2RedMin = 4096;

	SpO2SamplesBetweenPulses = 0;
	
	CurrentRedValue = INITIAL_CURRENT;
	CurrentIRValue = INITIAL_CURRENT;
	
	SpO2ValidValue = 0;
		
	SpO2ActualSubStateSubState = 0;	
		
	TimerToRecalibrate = 0;
	

	//clean arrays
	for (i=0; i<SPO2_ARRAY_LENGTH; i++){

		CopyOfSpO2HeartRateArray[i] = 0;
		CopyOfSpO2SpO2Array[i] = 0;
		
		SpO2HeartRateArray[i] = 0;
		SpO2SpO2Array[i] = 0;
	}
}
