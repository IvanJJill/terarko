#include "iolib_is_channel.h"
#include "utils.h"
#include "PLL.c"

// 2. Declarations Section
//   Global Variables
// Define pins(bits 0..8) for port F
#define PORT_NAME F
#define CHANNELS \
    CH(SW2, 0, In, Dig, NA, On) \
    CH(Red, 1, Out, Dig, NA, Off) \
    CH(Blue, 2, Out, Dig, NA, Off) \
	CH(Green, 3, Out, Dig, NA, Off) \
    CH(SW1, 4, In, Dig, NA, On)
#include "init_is.h"

#define PORT_NAME D
#define CHANNELS \
    CH(PB01, 0, In, Dig, NA, Off) \
	CH(PB02, 1, In, Dig, NA, Off) \
    CH(PB03, 2, In, Dig, NA, Off) \
	CH(PB04, 3, In, Dig, NA, Off)
#include "init_is.h"

#define PORT_NAME A
#define CHANNELS \
CH(UVLamp01, 2, Out, Dig, NA, Off)\
CH(LightLamp01, 4, Out, Dig, NA, Off)\
CH(Pump02, 5, Out, Dig, NA, Off)\
CH(Pump01, 6, Out, Dig, NA, Off)
#include "init_is.h"

//CH(Light01, 0, In, Anl, NA, Off)\

#define PORT_NAME E
#define CHANNELS \
CH(Temp01, 1, In, Anl, NA, Off)\
CH(Valve01, 2, Out, Dig, NA, Off)\
CH(Valve02, 3, Out, Dig, NA, Off)
#include "init_is.h"
/* NOKIA 5110 DISPLAY UTILS */
void N5110_Init();

/***=======NOKIA 5110 DISPLAY=======***/
// Nokia 5110 screen pins port C
//#define PORT_NAME C
//#define CHANNELS \
//CH(RST, 4, Out, Dig, SSI, Off)\
//CH(CE, 5, Out, Dig, SSI, Off)\
//CH(DC, 6, Out, Dig, SSI, Off)\
//CH(DIN, 7, Out, Dig, SSI, Off)
//#include "init_is.h"

// Nokia 5110 screen pins port D
//#define PORT_NAME D
//#define CHANNELS \
//CH(CLK, 6, Out, Dig, SSI, Off)\
//#include "init_is.h"
/***=======NOKIA 5110 DISPLAY=======***/


// initialize a timer A0 x100ms
#define TIMER_NAME TA
#define TIMERS \
 TM(0, 10)\
 TM(1, 20)\
 TM(2, 5)
#include "timer_init.h"

// make a schedule for devices
// every line has starting time and running time
schedule Pump01[]= {
		{{7,15,0},{0,0,3}},
		{{9,28,0},{0,0,3}},
		{{10,58,0},{0,0,3}},
		{{12,28,0},{0,0,3}},
		{{15,28,0},{0,0,3}},
};
schedule UVLamp01[] = {
		{{7,30,0},{1,31,0}},
		{{9,30,0},{1,31,0}},
		{{11,0,0},{0,31,0}},
		{{12,30,0},{0,31,0}},
		{{15,30,0},{0,31,0}}
};
schedule LightLamp01[] = {
		{{7,0,0},{0,31,0}},
		{{9,0,0},{0,31,0}},
		{{11,30,0},{1,00,0}},
		{{13,00,0},{2,31,0}},
		{{16,00,0},{2,00,0}}
};
void HW_Init(){
	 F_Init();
	 A_Init();
	 D_Init();
	 E_Init();
	 PLL_Init();
	 TA0_Init();
	 TA1_Init();
	 TA2_Init();
//	 N5110_Init();
}
void main(){
	HW_Init();
	BOOL mem[16];
	time s = {00,00,00}; //
	BOOL UVLampON = FALSE;
	BOOL LightLampON = FALSE;
	BOOL PumpON = FALSE;

	unsigned long AnlVal = 100;
	 // ToDo - write read from memory to the seconds counter
	 while(1){
		 AnlVal = LOAD(E_Temp01);
		 // Set the time by PB01 PB02
		 if(NF(D_PB01,&mem[0])){
			 s.hh++;
		 }
		 if(NF(D_PB02,&mem[1])){
			 s.mm += 10;
		 }

		 if (TA0_GET()) {TA0_CLR(); ++s.ss; day_time(&s);}

		 // activate UV Lamp by schedule
		 UVLampON = time_start_end_arr(UVLamp01, &s, 5);
		 if(UVLampON) SET(A_UVLamp01); else	CLR(A_UVLamp01);

		 // Activate Light Lamp by schedule
		 LightLampON = time_start_end_arr(LightLamp01, &s, 5);
		 if(LightLampON) SET(A_LightLamp01); else CLR(A_LightLamp01);
//
//		 PumpON = time_start_end_arr(Pump01, &s, 5);
//		 if(PumpON) SET(A_Pump01); else	CLR(A_Pump01); // || !GET(F_SW2)

		 // Activate Pump2 (inlet pump) on PB3 push
		 // Deactivate on a 2nd push
		 if (NF(D_PB04, &mem[2])) {
			 TGL(A_Pump01);
		 }

		 // Activate Pump2 (inlet pump) on PB3 push
		 // Deactivate on a 2nd push
		 if (NF(D_PB03, &mem[3])) {
			 TGL(A_Pump02);
		 }
		 //if (s.ss % 2 ==0) {SET(A_Pump01);CLR(A_LightLamp01);}
		 //else {CLR(A_Pump01);SET(A_LightLamp01);;}
	// ToDo - write store in memory of the seconds counter
	 }
}

void N5110_Init(){
//	CLR(C_RST);
//	SET(C_RST);
}
