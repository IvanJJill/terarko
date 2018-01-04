/*
 * timer_init.h
 *
 *  Created on: Jan 1, 2017
 *      Author: isatsiuk
 */

#ifndef TIMERS
    #error "TIMERS not defined"
#endif
#ifndef TIMER_NAME
#error "TIMER_NAME not defined"
#endif

#define CAT_NOEXPAND(A, B) A ## B
#define CAT(A, B) CAT_NOEXPAND(A, B)

#define TM(a, b) CAT(TIMER_NAME, a) = a,
enum TIMER_NAME {
    TIMERS
#undef TM
};

/* time for 100ms based on 16MHz */
/* reset the 0th bit of this register (TAEN)*/
/* reset the whole register */
/* set periodic TAMR */
/* reset bit 4 (TACDIR) to count down */
/* set the 0th bit of this register (TAEN) */
#define TM(a, b)\
void CAT(CAT(TIMER_NAME, a),_Init)(void){\
    unsigned int time_preset = b * 0x00186A00;\
    unsigned int tmr_no = a;\
    VLTL((RCGCTIMER + _TMREN)) |= 0x1<<a;\
	VLTL((TIMER_BASE[tmr_no] + GPTMCTL)) &= ~0x1;\
    VLTL((TIMER_BASE[tmr_no] + GPTMCFG)) = 0x0;\
    VLTL((TIMER_BASE[tmr_no] + GPTMTAMR)) |= 0x2;\
    VLTL((TIMER_BASE[tmr_no] + GPTMTAMR)) &= ~(0x1<<4);\
    VLTL((TIMER_BASE[tmr_no] + GPTMTAILR)) = time_preset;\
    VLTL((TIMER_BASE[tmr_no] + GPTMCTL)) |= 0x1;}\

TIMERS
#undef TM

#define TM(a,b) \
BOOL CAT(CAT(CAT(TIMER_NAME,a),_),GET)(void){	return ((*((BOOL *)(TIMER_BASE[a] + GPTMRIS))) && 0x1);}\

TIMERS
#undef TM

#define TM(a,b) \
void CAT(CAT(CAT(TIMER_NAME,a),_),CLR)(void){	VLTL((TIMER_BASE[a] + GPTMICR)) |= 0x1;}\

TIMERS
#undef TM

#undef TIMERS
#undef TIMER_NAME
#undef CAT_NOEXPAND
#undef CAT
