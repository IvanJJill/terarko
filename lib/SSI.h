#ifndef SSI_H_
#define SSI_H_

// Module to enable SSI
// originally made to communicate with Nokia 5110 screen

volatile unsigned long * SSI_CR = {0x40008004, 0x40009004, 0x4000A004, 0x4000B004};
volatile unsigned long * SSI_CC = {0x40008FC8, 0x40009FC8, 0x4000AFC8, 0x4000BFC8};
volatile unsigned long * SSI_CPSR = {0x40008010, 0x40009010, 0x4000A010, 0x4000B010};

#define RCGCSSI VLTL(0x400FE61C); // register for SSI control
#define RCGCGPIO VLTL(0x400FE608);
#define SSICR1 VLTL();

void setbit(unsigned long * addr, short bit){
	addr |= (1<<bit);
}

SSI_Init(short SSI_no){
	int port = 0;
	switch (SSI_no){
	case 2 : 
		port = 1;
		break;
	case 1 : 
		port = 3;
		break;
	case 0:
	default: 
		port = 0;
		break;
	}
	
	// Step 1
	RCGCSSI = (1 << SSI_no); // specify the SSI number to use
	// Step 2
	RCGCGPIO = ( 1 << port); // specify the Port that will be SSI enabled
	// Step 3
	// Enable ALT action - done on the IO-Channel configuration itself
	
	// Step 4
	// Set ALT action to be SSI - done on the IO-Channel configuration itself
	
	// Step 5
	// Set Dig function also done in IO-Channel config itself
	
	// ===== Configure frame formats ===== //
	// Step 1
	// clear in SSICRx an SSE bit (bit 1)
	VLTL(SSI_CR[SSI_no]) &= ~(1<<1);
	//Step 2: Select whether the SSI is a master or slave
	VLTL(SSI_CR[SSI_no]) = 0x00000000;
	// Step 3 Configure SSI Clock by writing to SSICC register
	VLTL(SSI_CC[SSI_no]) = 0x00000000;
	// Step 4 Configure the clock prescale divisor by writing the SSICPSR register
	VLTL(SSI_CPSR[SSI_no]) = 10;
	// Step 5 Write the SSICR0 register with the size of data transferred by one set (we do 8 bit)
	VLTL(SSI_CR[SSI_no]) = 0x7;
	// Step 6 Set in SSICRx an SSE bit (bit 1)
	VLTL(SSI_CR[SSI_no]) |= (1<<1);
	
}


#endif