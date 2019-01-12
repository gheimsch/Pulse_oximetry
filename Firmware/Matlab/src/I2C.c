#include "I2C.h"

/* Private variables */
static uint32_t I2C_Timeout;
static uint32_t I2C_INT_Clocks[3] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

/* Private defines */
#define I2C_TRANSMITTER_MODE   0
#define I2C_RECEIVER_MODE      1
#define I2C_ACK_ENABLE         1
#define I2C_ACK_DISABLE        0

/* Private functions */
static void I2C1_INT_InitPins(I2C_PinsPack_t pinspack);
static void I2C2_INT_InitPins(I2C_PinsPack_t pinspack);
static void I2C3_INT_InitPins(I2C_PinsPack_t pinspack);

void Init_I2C(I2C_TypeDef* I2Cx, I2C_PinsPack_t pinspack, uint32_t clockSpeed) {
	I2C_InitTypeDef I2C_InitStruct;
	
	if (I2Cx == I2C1) {
		/* Enable clock */
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
		
		/* Enable pins */
		I2C1_INT_InitPins(pinspack);
		
		/* Check clock, set the lowest clock your devices support on the same I2C but */
		if (clockSpeed < I2C_INT_Clocks[0]) {
			I2C_INT_Clocks[0] = clockSpeed;
		}
		
		/* Set values */
		I2C_InitStruct.I2C_ClockSpeed = I2C_INT_Clocks[0];
		I2C_InitStruct.I2C_AcknowledgedAddress = I2C1_ACKNOWLEDGED_ADDRESS;
		I2C_InitStruct.I2C_Mode = I2C1_MODE;
		I2C_InitStruct.I2C_OwnAddress1 = I2C1_OWN_ADDRESS;
		I2C_InitStruct.I2C_Ack = I2C1_ACK;
		I2C_InitStruct.I2C_DutyCycle = I2C1_DUTY_CYCLE;
	} else if (I2Cx == I2C2) {
		/* Enable clock */
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		
		/* Enable pins */
		I2C2_INT_InitPins(pinspack);
		
		/* Check clock, set the lowest clock your devices support on the same I2C but */
		if (clockSpeed < I2C_INT_Clocks[1]) {
			I2C_INT_Clocks[1] = clockSpeed;
		}
		
		/* Set values */
		I2C_InitStruct.I2C_ClockSpeed = I2C_INT_Clocks[1];
		I2C_InitStruct.I2C_AcknowledgedAddress = I2C2_ACKNOWLEDGED_ADDRESS;
		I2C_InitStruct.I2C_Mode = I2C2_MODE;
		I2C_InitStruct.I2C_OwnAddress1 = I2C2_OWN_ADDRESS;
		I2C_InitStruct.I2C_Ack = I2C2_ACK;
		I2C_InitStruct.I2C_DutyCycle = I2C2_DUTY_CYCLE;
	} else if (I2Cx == I2C3) {
		/* Enable clock */
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
		
		/* Enable pins */
		I2C3_INT_InitPins(pinspack);
		
		/* Check clock, set the lowest clock your devices support on the same I2C but */
		if (clockSpeed < I2C_INT_Clocks[2]) {
			I2C_INT_Clocks[2] = clockSpeed;
		}
		
		/* Set values */
		I2C_InitStruct.I2C_ClockSpeed = I2C_INT_Clocks[2];
		I2C_InitStruct.I2C_AcknowledgedAddress = I2C3_ACKNOWLEDGED_ADDRESS;
		I2C_InitStruct.I2C_Mode = I2C3_MODE;
		I2C_InitStruct.I2C_OwnAddress1 = I2C3_OWN_ADDRESS;
		I2C_InitStruct.I2C_Ack = I2C3_ACK;
		I2C_InitStruct.I2C_DutyCycle = I2C3_DUTY_CYCLE;
	}
	

	/* Disable I2C first */
	I2Cx->CR1 &= ~I2C_CR1_PE;
	
	/* Initialize I2C */
	I2C_Init(I2Cx, &I2C_InitStruct);
	
	/* Enable I2C */
	I2Cx->CR1 |= I2C_CR1_PE;
}

uint8_t I2C_Read(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg) {
	uint8_t received_data;
	I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	I2C_WriteData(I2Cx, reg);
	I2C_Stop(I2Cx);
	I2C_Start(I2Cx, address, I2C_RECEIVER_MODE, I2C_ACK_DISABLE);
	received_data = I2C_ReadNack(I2Cx);
	return received_data;
}

void I2C_ReadMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_ENABLE);
	I2C_WriteData(I2Cx, reg);
	I2C_Stop(I2Cx);
	I2C_Start(I2Cx, address, I2C_RECEIVER_MODE, I2C_ACK_ENABLE);
	while (count--) {
		if (!count) {
			/* Last byte */
			*data++ = I2C_ReadNack(I2Cx);
		} else {
			*data++ = I2C_ReadAck(I2Cx);
		}
	}
}

uint8_t I2C_ReadNoRegister(I2C_TypeDef* I2Cx, uint8_t address) {
	uint8_t data;
	I2C_Start(I2Cx, address, I2C_RECEIVER_MODE, I2C_ACK_ENABLE);
	/* Also stop condition happens */
	data = I2C_ReadNack(I2Cx);
	return data;
}

void I2C_ReadMultiNoRegister(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data, uint16_t count) {
	I2C_Start(I2Cx, address, I2C_RECEIVER_MODE, I2C_ACK_ENABLE);
	while (count--) {
		if (!count) {
			/* Last byte */
			*data = I2C_ReadNack(I2Cx);
		} else {
			*data = I2C_ReadAck(I2Cx);
		}
	}
}

void I2C_Write(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data) {
	I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	I2C_WriteData(I2Cx, reg);
	I2C_WriteData(I2Cx, data);
	I2C_Stop(I2Cx);
}

void I2C_WriteMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	I2C_WriteData(I2Cx, reg);
	while (count--) {
		I2C_WriteData(I2Cx, *data++);
	}
	I2C_Stop(I2Cx);
}

void I2C_WriteNoRegister(I2C_TypeDef* I2Cx, uint8_t address, uint8_t data) {
	I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	I2C_WriteData(I2Cx, data);
	I2C_Stop(I2Cx);
}

void I2C_WriteMultiNoRegister(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data, uint16_t count) {
	I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	while (count--) {
		I2C_WriteData(I2Cx, *data++);
	}
	I2C_Stop(I2Cx);
}


uint8_t I2C_IsDeviceConnected(I2C_TypeDef* I2Cx, uint8_t address) {
	uint8_t connected = 0;
	/* Try to start, function will return 0 in case device will send ACK */
	if (!I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_ENABLE)) {
		connected = 1;
	}
	
	/* STOP I2C */
	I2C_Stop(I2Cx);
	
	/* Return status */
	return connected;
}

void I2C_InitCustomPinsCallback(I2C_TypeDef* I2Cx, uint16_t AlternateFunction) {
	/* Custom user function. */
	/* In case user needs functionality for custom pins, this function should be declared outside this library */
}

/* Private functions */
int16_t I2C_Start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction, uint8_t ack) {
	/* Generate I2C start pulse */
	I2Cx->CR1 |= I2C_CR1_START;
	
	/* Wait till I2C is busy */
	I2C_Timeout = I2C_TIMEOUT;
	while (!(I2Cx->SR1 & I2C_SR1_SB)) {
		if (--I2C_Timeout == 0x00) {
			return 1;
		}
	}

	/* Enable ack if we select it */
	if (ack) {
		I2Cx->CR1 |= I2C_CR1_ACK;
	}

	/* Send write/read bit */
	if (direction == I2C_TRANSMITTER_MODE) {
		/* Send address with zero last bit */
		I2Cx->DR = address & ~I2C_OAR1_ADD0;
		
		/* Wait till finished */
		I2C_Timeout = I2C_TIMEOUT;
		while (!(I2Cx->SR1 & I2C_SR1_ADDR)) {
			if (--I2C_Timeout == 0x00) {
				return 1;
			}
		}
	}
	if (direction == I2C_RECEIVER_MODE) {
		/* Send address with 1 last bit */
		I2Cx->DR = address | I2C_OAR1_ADD0;
		
		/* Wait till finished */
		I2C_Timeout = I2C_TIMEOUT;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
			if (--I2C_Timeout == 0x00) {
				return 1;
			}
		}
	}
	
	/* Read status register to clear ADDR flag */
	I2Cx->SR2;
	
	/* Return 0, everything ok */
	return 0;
}

void I2C_WriteData(I2C_TypeDef* I2Cx, uint8_t data) {
	/* Wait till I2C is not busy anymore */
	I2C_Timeout = I2C_TIMEOUT;
	while (!(I2Cx->SR1 & I2C_SR1_TXE) && I2C_Timeout) {
		I2C_Timeout--;
	}
	
	/* Send I2C data */
	I2Cx->DR = data;
}

uint8_t I2C_ReadAck(I2C_TypeDef* I2Cx) {
	uint8_t data;
	
	/* Enable ACK */
	I2Cx->CR1 |= I2C_CR1_ACK;
	
	/* Wait till not received */
	I2C_Timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
		if (--I2C_Timeout == 0x00) {
			return 1;
		}
	}
	
	/* Read data */
	data = I2Cx->DR;
	
	/* Return data */
	return data;
}

uint8_t I2C_ReadNack(I2C_TypeDef* I2Cx) {
	uint8_t data;
	
	/* Disable ACK */
	I2Cx->CR1 &= ~I2C_CR1_ACK;
	
	/* Generate stop */
	I2Cx->CR1 |= I2C_CR1_STOP;
	
	/* Wait till received */
	I2C_Timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
		if (--I2C_Timeout == 0x00) {
			return 1;
		}
	}

	/* Read data */
	data = I2Cx->DR;
	
	/* Return data */
	return data;
}

uint8_t I2C_Stop(I2C_TypeDef* I2Cx) {
	/* Wait till transmitter not empty */
	I2C_Timeout = I2C_TIMEOUT;
	while (((!(I2Cx->SR1 & I2C_SR1_TXE)) || (!(I2Cx->SR1 & I2C_SR1_BTF)))) {
		if (--I2C_Timeout == 0x00) {
			return 1;
		}
	}
	
	/* Generate stop */
	I2Cx->CR1 |= I2C_CR1_STOP;
	
	/* Return 0, everything ok */
	return 0;
}




/* Private functions */
static void I2C1_INT_InitPins(I2C_PinsPack_t pinspack) {
	/* Init pins */
	GPIO_InitTypeDef GPIO_InitStruct;

#if defined(GPIOB)
	if (pinspack == I2C_PinsPack_1) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
	}
#endif
#if defined(GPIOB)
	if (pinspack == I2C_PinsPack_2) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	}
#endif
#if defined(GPIOB)
	if (pinspack == I2C_PinsPack_3) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	}
#endif
	if (pinspack == I2C_PinsPack_Custom) {
		/* Init custom pins, callback function */
		I2C_InitCustomPinsCallback(I2C1, GPIO_AF_I2C1);
	}
}

static void I2C2_INT_InitPins(I2C_PinsPack_t pinspack) {
	/* Init pins */
	GPIO_InitTypeDef GPIO_InitStruct;

#if defined(GPIOB)
	if (pinspack == I2C_PinsPack_1) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);
	}
#endif
#if defined(GPIOF)
	if (pinspack == I2C_PinsPack_2) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOF, &GPIO_InitStruct);
		GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF_I2C2);
		GPIO_PinAFConfig(GPIOF, GPIO_PinSource11, GPIO_AF_I2C2);
	}
#endif
#if defined(GPIOH)
	if (pinspack == I2C_PinsPack_3) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOH, &GPIO_InitStruct);
		GPIO_PinAFConfig(GPIOH, GPIO_PinSource10, GPIO_AF_I2C2);
		GPIO_PinAFConfig(GPIOH, GPIO_PinSource11, GPIO_AF_I2C2);
	}
#endif
	if (pinspack == I2C_PinsPack_Custom) {
		/* Init custom pins, callback function */
		I2C_InitCustomPinsCallback(I2C2, GPIO_AF_I2C2);
	}
}

static void I2C3_INT_InitPins(I2C_PinsPack_t pinspack) {
	/* Init pins */
	GPIO_InitTypeDef GPIO_InitStruct;

#if defined(GPIOA) && defined(GPIOC)
	if (pinspack == I2C_PinsPack_1) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_I2C3);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOC, &GPIO_InitStruct);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_I2C3);
	}
#endif
#if defined(GPIOH)
	if (pinspack == I2C_PinsPack_2) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOH, &GPIO_InitStruct);
		GPIO_PinAFConfig(GPIOH, GPIO_PinSource10, GPIO_AF_I2C3);
		GPIO_PinAFConfig(GPIOH, GPIO_PinSource11, GPIO_AF_I2C3);
	}
#endif
	if (pinspack == I2C_PinsPack_Custom) {
		/* Init custom pins, callback function */
		I2C_InitCustomPinsCallback(I2C3, GPIO_AF_I2C3);
	}
}
