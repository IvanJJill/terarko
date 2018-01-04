/*
 * init_is.h
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

#define CH(a, b, d, t, l, pr) CAT(CAT(PORT_NAME,_), a) = PORT_NAME * 10 + b,
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
    unsigned int da_anl = 0; // digital or analog
    unsigned int reg = 0; // regular function on/off
    unsigned int pull = 0;// pull-up resistor on/off
    unsigned int pctl = 0; // PCTL_R register
    unsigned long r_val = 0;
    Ports port = PORT_NAME;
    unsigned int channel = 0;

#define CH(a, b, d, t, l, pr)\
			channel = port * 10 + b;\
			act_ch |= 0x1<<b;\
			io |= d ? 0x1<<b : 0;\
			da |= t ? 0 : 0x1<<b;\
			da_anl |= t ? 0x1<<b : 0;\
    		reg |= l ? 0x1<<b : 0;\
    		pull |= pr ? 0x1<<b : 0;\
    		pctl |= l != NA ? (l)<<(4*b) : 0;
    CHANNELS
#undef CH
    SYSCTL_RCGC2_R |= 0x1<<port;      		// 1) Activate Port
    long delay = SYSCTL_RCGC2_R;      	// allow time to activate port
    VLTL((PORT_BASE[port] + _PUR_R)) |= pull; // 8) enable pull-up on selected channel
    VLTL((PORT_BASE[port] + _DIR_R)) |= io; // 5) input/output on selected channel

    // if Analogue is Enabled on channel
    if(da_anl){
    	SYSCTL_RCGC2_R |= 0x00000010;      		// 1) Activate Port
		delay = SYSCTL_RCGC2_R;      	// allow time to activate port

//		VLTL((PORT_BASE[port] + _PCTL_R)) &= ~pctl ; // 4) enable regular GPIO on selected channel
    	VLTL((PORT_BASE[port] + _AMSEL_R)) |= da; 	//3) enable analog function on selected channel
    	VLTL((PORT_BASE[port] + _DEN_R)) &= ~da; // 7) disable digital on selected channel
    	VLTL((PORT_BASE[port] + _AFSEL_R)) |= reg; // 6) alternate function on selected channel

		SYSCTL_RCGC0_R |= 0x00010000;   // 6) activate ADC0
		delay = SYSCTL_RCGC2_R;
    	// ADC Configuration / Initialization
//    	VLTL((RCGCADC_R + _ADCEN)) |= 0x1;// 6) activate ADC0
//    	VLTL((RCGCADC_R + _ADCCLK)) &= ~0x00000300;// 7) Set sample sequencer clock to 125K
    	SYSCTL_RCGC0_R &= ~0x00000300;  // 7) configure for 125K

//    	while(r_val == 0){
//    		r_val = VLTL((ADC_BASE[0] + _ADCACTSS));
//    		r_val = r_val >> 16;
//    		r_val &= 0x0001;
//    	}; // wait for busy flag to be 0

    	VLTL((ADC_BASE[0] + _ADCSSPRI)) = 0x0123; // 10) Set higher priority for SS3
    	VLTL((ADC_BASE[0] + _ADCACTSS)) &= ~0x0008;//~(0x1 << SAMPSEQ); // 8) Disable sample sequencer
    	VLTL((ADC_BASE[0] + _ADCEMUX)) &= ~0xF000; // 9) Use Software to read sequencer
    	VLTL((ADC_BASE[0] + _ADCSSMUX[SAMPSEQ])) |= 0x1 << getAIN(channel); // 11) Set corresponding Channel for current Sample sequencer
    	VLTL((ADC_BASE[0] + _ADCSSCTL[SAMPSEQ])) = 0x0006; // 12) no TS0 D0, yes IE0 END0

    	VLTL((ADC_BASE[0] + _ADCACTSS)) |= 0x1 << SAMPSEQ; // 13) Enable sample sequencer

    } else {
        VLTL((PORT_BASE[port] + _LOCK_R))	= 0x4C4F434B;   // 2) unlock GPIO Port
        VLTL((PORT_BASE[port] + _CR_R)) = act_ch; // enable changes on pins
		VLTL((PORT_BASE[port] + _AMSEL_R)) &= ~da; 	//3) disable analog function on selected channel
		VLTL((PORT_BASE[port] + _PCTL_R)) &= ~pctl ; // 4) enable regular GPIO on selected channel
		VLTL((PORT_BASE[port] + _AFSEL_R)) &= ~reg; // 6) regular function on selected channel
		VLTL((PORT_BASE[port] + _DEN_R)) |= da; // 7) enable digital on selected channel
    }
}

#undef CHANNELS
#undef PORT_NAME
#undef CAT_NOEXPAND
#undef CAT
