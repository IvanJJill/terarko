/*
 * iolib_is.h
 *
 *  Created on: Jan 1, 2017
 *      Author: i satsiuk
 *
 *  This library will serve to ease standard IO
 *  functions for Launchpad
 */

#ifndef IOLIB_IS_H_
#define IOLIB_IS_H_

// IO enable register
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC0_R			(*((volatile unsigned long *)0x400FE100))
#define VLTL(a) (*((volatile unsigned long *) a))

typedef unsigned char BOOL;
typedef unsigned char ADDR;
#define TRUE  1
#define FALSE 0

// Sample sequence for Analogue reading
#define SAMPSEQ 3
// Ports
typedef enum PORTS {
    A, B, C, D, E, F
} Ports;
enum DIR {In, Out};
enum TYP {Dig, Anl};
enum ONOFF {Off, On};

// enum for IO alternative functions
enum ALT_FUNC{NA, UART = 1, SSI, I2C0, JTAG = 1};

#ifndef IOLIB_IS_H_

// Port's bases
const unsigned long PORT_BASE [6] = {
  0x42008000,
  0x420A0000,
  0x420C0000,
  0x420E0000,
  0x42480000,
  0x424A0000
};

// Bit # * 4
const unsigned long PORT_BIT [8] = {
  0x00,
  0x04,
  0x08,
  0x0C,
  0x10,
  0x14,
  0x18,
  0x1C
};

#else
// Port's bases
const unsigned long PORT_BASE [6] = {
  0x40004000,
  0x40005000,
  0x40006000,
  0x40007000,
  0x40024000,
  0x40025000
};

//Offsets for init memory
#define _DIR_R 		0x0400
#define _AFSEL_R 	0x0420
#define _PUR_R		0x0510
#define _DEN_R 		0x051C
#define _LOCK_R		0x0520
#define _CR_R		0x0524
#define _AMSEL_R 	0x0528
#define _PCTL_R 	0x052C

// Ports
typedef enum TIMERS {
    TA, TB
} Timers;
// Timer's bases
const unsigned long TIMER_BASE [6] = {
  0x40030000,
  0x40031000,
  0x40032000,
  0x40033000,
  0x40034000,
  0x40035000
};
// Timers Enable register
#define RCGCTIMER 	0x400FE000
#define _TMREN		0x0604
//Offsets for Timer control registers
#define GPTMCFG		0x0000
#define GPTMTAMR 	0x0004
#define GPTMCTL		0x000C
#define GPTMTAILR	0x0028
#define GPTMRIS		0x001C
#define GPTMICR		0x0024


// ------ ANALOGUE INPUTS ------
enum AIN2CH{
	AIN00 = 43,
	AIN01 = 42,
	AIN02 = 41,
	AIN03 = 40,
	AIN04 = 33,
	AIN05 = 32,
	AIN06 = 31,
	AIN07 = 30,
	AIN08 = 45,
	AIN09 = 44,
	AIN10 = 14,
	AIN11 = 15
};

int AIN2CH_table[12] = {AIN00,AIN01,AIN02,AIN03,AIN04,AIN05,AIN06,AIN07,AIN08,AIN09,AIN10,AIN11};

int getAIN (ADDR channel){
	int i = 0;
	for (;i<12;i++){
		if (channel == AIN2CH_table[i]) return i;
	}
	return 0;
}

// ADC Configure registers
#define RCGCADC_R	0x400FE000
#define _ADCEN		0x638
#define _ADCCLK		0x100

// This is not purely for ADC, but for other functionality as well
const unsigned long ADC_BASE [2] = {
		0x40038000,
		0x40039000
};

// ADC Configuration registers offset from ADC_BASE
#define _ADCACTSS	0x000
#define _ADCTSSEL	0x01C
#define _ADCEMUX	0x014
#define _ADCSSPRI	0x020

// This one is for reading value after sampling
#define _ADCPSSI	0x0028
#define _ADCRIS		0x0004
#define _ADCISC		0x000C
const unsigned long _ADCSSMUX[4] = {0x0040,	0x0060,	0x0080, 0x00A0};
const unsigned long _ADCSSCTL[4] = {0x0044,	0x0064,	0x0084, 0x00A4};
const unsigned long _ADCSSFIFO[4] = {0x0048, 0x0068, 0x0088, 0x00A8};

unsigned long LOAD(ADDR channel){
	unsigned long result;
	unsigned long flag = 0;
	VLTL((ADC_BASE[0] + _ADCACTSS)) = 0x1 << SAMPSEQ;            // 1) initiate SS3
	// 2) wait for conversion done
	while(flag == 0){
		flag = VLTL((ADC_BASE[0] + _ADCRIS)) & (0x1 << SAMPSEQ);
	};

	result = VLTL((ADC_BASE[0] + _ADCSSFIFO[4])) & 0xFFF;   // 3) read result
	VLTL((ADC_BASE[0] + _ADCISC)) = 0x1 << SAMPSEQ;             // 4) acknowledge completion
	return result;
}

// Bit # * 4
const unsigned long PORT_BIT [8][2] = {
  {0x0004, 0x0001},
  {0x0008, 0x0002},
  {0x0010, 0x0004},
  {0x0020, 0x0008},
  {0x0040, 0x0010},
  {0x0080, 0x0020},
  {0x0100, 0x0040},
  {0x0200, 0x0080}
};
#endif
//   Function Prototypes
//

// Delay function, creating a delay in multiple of 100 msecs
void delay(unsigned long msec);
void delay10(unsigned long msec);
/* Bit Bandig
// bit_word_offset = (byte_offset x 32) + (bit_number x 4)
// bit_word_addr = bit_band_base + bit_word_offset
//#define BT_Offs
//#define Port1_Base 0x20000000
//#define Port2_Base 0x40000000
//#define BB1_Base 0x22000000
//#define BB2_Base 0x42000000
// Base addresses for GPIO ports
GPIO_DATA:
  A = 0x40004000
  B = 0x40005000
  C = 0x40006000
  D = 0x40007000
  E = 0x40024000
  F = 0x40025000

// Byte offset
GPIO_B_OFFS:
  A = 0x00004000
  B = 0x00005000
  C = 0x00006000
  D = 0x00007000
  E = 0x00024000
  F = 0x00025000

// Byte offset * 32
GPIO_B_OFFS:
  A = 0x00008000
  B = 0x000A0000
  C = 0x000C0000
  D = 0x000E0000
  E = 0x00480000
  F = 0x004A0000

// Base + Byte offset * 32
GPIO_B_OFFS:
  A = 0x42008000
  B = 0x420A0000
  C = 0x420C0000
  D = 0x420E0000
  E = 0x42480000
  F = 0x424A0000

Bits:
Bit0 = 0x00
Bit1 = 0x04
Bit2 = 0x08
Bit3 = 0x0C
Bit4 = 0x10
Bit5 = 0x14
Bit6 = 0x18
Bit7 = 0x1C
  */

// Struct of Channel includes port and number of channel
// currtntly only used by SET_Mul, and FSM, but can be used for others
typedef struct tSet{
	ADDR chanl;
} toSet;

//#define GET(a,b) (*((BOOL *)(PORT_BASE[a] + PORT_BIT[b][0])))
BOOL GET(const ADDR ch){
	ADDR port = ch / 10;
	ADDR bit = ch % 10;
	BOOL tmp = (*((BOOL *)(PORT_BASE[port] + PORT_BIT[bit][0])))>>bit;
	delay10(2); // delay 20 ms
	if (tmp == (*((BOOL *)(PORT_BASE[port] + PORT_BIT[bit][0])))>>bit) return tmp;
  return FALSE;
}

BOOL GETI(const enum PORTS port, const char bit){
  return (*((BOOL *)(PORT_BASE[port] + PORT_BIT[bit][0]))) ^ 0x1;
}

void SET(const ADDR ch){
	ADDR port = ch / 10;
	ADDR bit = ch % 10;
	(*((BOOL *)(PORT_BASE[port] + PORT_BIT[bit][0]))) = PORT_BIT[bit][1];
}

// Set multiple channels
void SET_Mul(toSet arr[]){
	toSet * iter;
	toSet * last;
	iter = arr;
	last = iter + sizeof(arr)/sizeof(arr[0]);
	while (iter <= last){
		SET(iter->chanl);++iter;
	}
}

void CLR(const ADDR ch){
	(*((BOOL *)(PORT_BASE[ch / 10] + PORT_BIT[ch % 10][0]))) = 0x0;
}
void TGL(const ADDR ch){
	ADDR port = ch / 10;
	ADDR bit = ch % 10;
	(*((BOOL *)(PORT_BASE[port] + PORT_BIT[bit][0]))) ^= PORT_BIT[bit][1];
}

void CLR_Mul(toSet arr[]){
	toSet * iter;
	toSet * last;
	iter = arr;
	last = iter + sizeof(arr)/sizeof(arr[0]);
	while (iter <= last){
		CLR(iter->chanl);++iter;
	}
}

void FLSH_Mul(toSet arr[], const int cnt){
	toSet * iter;
	toSet * last;
	iter = arr;
	last = iter + sizeof(arr)/sizeof(arr[0]);
	while (iter <= last){
		if(cnt % 2 == 0) CLR(iter->chanl);
		else SET(iter->chanl);
		++iter;
	}
}


// Subroutine to delay in units of 100 mili seconds
// We will make a precise estimate later:
//   For now we assume it takes 1 sec to count down
//   from 4,000,000 down to zero
// Inputs: Number of mili seconds to delay
// Outputs: None
// Notes: ...
void delay(unsigned long msec){
  unsigned long count;

  while(msec > 0 ) { // repeat while there are still msecs to delay
    count = 133333; // it takes 133333 ops per 100ms
    while (count > 0) {
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}
void delay10(unsigned long msec){
  unsigned long count;

  while(msec > 0 ) { // repeat while there are still msecs to delay
    count = 13333; // it takes 13333 ops per 10ms
    while (count > 0) {
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}

#endif /* IOLIB_IS_H_ */
