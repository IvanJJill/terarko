/*
 * init.h
 *
 *  Created on: Jan 1, 2017
 *      Author: isatsiuk
 */

#ifndef CHANNELS
    #error "CHANNELS not defined"
#endif
#ifndef PORT_NAME
#error "PORT_NAME not defined"
#endif

#define CAT_NOEXPAND(A, B) A ## B
#define CAT(A, B) CAT_NOEXPAND(A, B)

#define CH(a, b, d, t, l, pr) CAT(CAT(PORT_NAME,_), a) = b,
enum PORT_NAME {
    CHANNELS
#undef CH
};

void CAT(PORT_NAME, _Init)(void){
#ifndef CHANNELS
    return;
#endif
    unsigned int act_ch = 0; // channels that are used e.g. 0, 1, 4 = b00010011 = 0x13
    unsigned int io = 0; // input or output
    unsigned int da = 0; // digital or analog
    unsigned int reg = 0; // regular function on/off
    unsigned int pull = 0;// pull-up resistor on/off
    unsigned int pctl = 0; // PCTL_R register
    Ports port = PORT_NAME;
#define CH(a, b, d, t, l, pr)\
			act_ch |= 0x1<<b;\
			io |= d ? 0x1<<b : 0;\
			da |= t ? 0 : 0x1<<b;\
    		reg |= l ? 0x1<<b : 0;\
    		pull |= pr ? 0x1<<b : 0;\
    		pctl |= b ? (0xF)<<(4*b) : 0;
    CHANNELS
#undef CH

    SYSCTL_RCGC2_R |= 0x1<<port;      		// 1) Activate Port
    long delay = SYSCTL_RCGC2_R;      	// allow time to activate port
    VLTL((PORT_BASE[port] + _LOCK_R))	= 0x4C4F434B;   // 2) unlock GPIO Port
    VLTL((PORT_BASE[port] + _CR_R)) = act_ch; // enable changes on pins
    VLTL((PORT_BASE[port] + _AMSEL_R)) &= ~da; 	//3) disable analog function on selected channels
    VLTL((PORT_BASE[port] + _PCTL_R)) &= ~pctl ; // 4) enable regular GPIO on selected channels
    VLTL((PORT_BASE[port] + _DIR_R)) |= io; // 5) input/output on selected channels
    VLTL((PORT_BASE[port] + _AFSEL_R)) &= ~reg; // 6) regular function on selected channels
    VLTL((PORT_BASE[port] + _DEN_R)) |= da; // 7) enable digital on selected channels
    VLTL((PORT_BASE[port] + _PUR_R)) |= pull; // 8) enable pull-up on selected channels
}

#undef CHANNELS
#undef PORT_NAME
#undef CAT_NOEXPAND
#undef CAT
