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

#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define VLTL(a) (*((volatile unsigned long *) a))

typedef unsigned char BOOL;
#define TRUE  1
#define FALSE 0
// Ports
typedef enum PORTS {
    A, B, C, D, E, F
} Ports;
enum DIR {In, Out};
enum TYP {Dig, Anl};
enum ONOFF {Off, On};

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
	enum PORTS p;
	char chanl;
} toSet;

//#define GET(a,b) (*((BOOL *)(PORT_BASE[a] + PORT_BIT[b][0])))
BOOL GET(const enum PORTS port, const char bit){
  return (*((BOOL *)(PORT_BASE[port] + PORT_BIT[bit][0])))>>bit;
}

BOOL GETI(const enum PORTS port, const char bit){
  return (*((BOOL *)(PORT_BASE[port] + PORT_BIT[bit][0]))) ^ 0x1;
}

void SET(const enum PORTS port, const char bit){
	(*((BOOL *)(PORT_BASE[port] + PORT_BIT[bit][0]))) = PORT_BIT[bit][1];
}

// Set multiple channels
void SET_Mul(toSet arr[]){
	toSet * iter;
	toSet * last;
	iter = arr;
	last = iter + sizeof(arr)/sizeof(arr[0]);
	while (iter <= last){
		SET(iter->p, iter->chanl);++iter;
	}
}

void CLR(const enum PORTS port, const char bit){
	(*((BOOL *)(PORT_BASE[port] + PORT_BIT[bit][0]))) = 0x0;
}

void CLR_Mul(toSet arr[]){
	toSet * iter;
	toSet * last;
	iter = arr;
	last = iter + sizeof(arr)/sizeof(arr[0]);
	while (iter <= last){
		CLR(iter->p, iter->chanl);++iter;
	}
}

void FLSH_Mul(toSet arr[], const int cnt){
	toSet * iter;
	toSet * last;
	iter = arr;
	last = iter + sizeof(arr)/sizeof(arr[0]);
	while (iter <= last){
		if(cnt % 2 == 0) CLR(iter->p, iter->chanl);
		else SET(iter->p, iter->chanl);
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

#endif /* IOLIB_IS_H_ */
