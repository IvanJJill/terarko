/*
 * utils.h
 *
 *  Created on: Jan 4, 2017
 *      Author: isatsiuk
 */
#ifndef UTILS_H_
#define UTILS_H_

#include "iolib_is_channel.h"

// Implement Set/Reset function by IO
void RS (const ADDR setr, const ADDR rsetr, const ADDR outr, BOOL* smem, BOOL* rmem){
	BOOL s=GET(setr);
	BOOL r=GET(rsetr);
	if (!s && !(*smem)) SET(outr);
	if (!r && !(*rmem)) CLR(outr);
	*smem = s;
	*rmem = r;
}
// Implement Set/Reset function by variables in memory
void RS_M (const BOOL setr, const BOOL rsetr, const ADDR outr, BOOL* smem, BOOL* rmem){
	BOOL s=setr;
	BOOL r=rsetr;
	if (s && !(*smem)) SET(outr);
	if (r && !(*rmem)) CLR(outr);
	*smem = s;
	*rmem = r;
}

// Positive Edge recognition
BOOL PF(const ADDR addr, BOOL* pfmem){
	BOOL val = GET(addr);
	BOOL ret = FALSE;
	if(val && !(*pfmem)) ret = TRUE;
	*pfmem = val;
	return ret;
}

// Negative edge recognition
BOOL NF(const ADDR addr, BOOL* pfmem){
	BOOL val = GET(addr);
	BOOL ret = FALSE;
	if(!val && *pfmem) ret = TRUE;
	*pfmem = val;
	return ret;
}

// =============== SCHEDULING ===================
typedef struct time{
	char hh;
	char mm;
	char ss;
} time;

typedef struct schedule{
	time start;
	time dur;
}schedule;

BOOL time_GT(time* lhs, time* rhs);
BOOL time_LT(time* lhs, time* rhs);
BOOL time_EQ(time* lhs, time* rhs);
time time_ADD(time* lhs, time* rhs);
time time_SUB(time* lhs, time* rhs);

/* Function will return TRUE when current time is within schedule time */
BOOL time_start_end(schedule* sch, time* s);

/* Function to go thhrough array of schedules and return TRUE if any is active*/
BOOL time_start_end_arr(schedule* sch,time *s, int amt);


void day_time(time *s){
	if (s->ss >= 60){
		 s->mm++;
		 s->ss = 0;
	 }
	 if (s->mm >= 60){
		 s->hh++;
		 s->mm = 0;
	 }
	 if (s->hh >= 23 && s->mm >= 59 && s->ss >= 59){
		 s->hh = 0;
		 s->mm = 0;
		 s->ss = 0;
	 }
}

void normalize(time * s){
	 if (s->ss >= 59){
		 s->mm++;
		 s->ss = s->ss % 60;
	 }
	 if (s->mm >= 59){
		 s->hh++;
		 s->mm = s->mm % 60;
	 }
	 if (s->hh >= 23){
		 s->hh = s->hh % 24;
	 }
}
// add two time values
time time_ADD(time* lhs, time* rhs){
	time tmp = {lhs->hh + rhs->hh, lhs->mm + rhs->mm, lhs->ss + rhs->ss};
	normalize(&tmp);
	return tmp;
}

// subtract two time values
time time_SUB(time* lhs, time* rhs){
	time tmp = {-1,-1,-1};
	if (time_GT(lhs, rhs)){
		tmp.hh = lhs->hh;tmp.mm = lhs->mm; tmp.ss = lhs->ss;
		if (lhs->ss < rhs->ss){
			tmp.mm--;
			tmp.ss += 60;
		}
		if(lhs->mm < rhs->mm){
			tmp.hh--;
			tmp.mm += 60;
		}
		tmp.hh -= rhs->hh; tmp.mm -= rhs->mm; tmp.ss -= rhs->ss;
		normalize(&tmp);
	}
	return tmp;
}

// time comparison
BOOL time_EQ(time*lhs, time*rhs){
	if (lhs->hh == rhs->hh && lhs->mm == rhs->mm && lhs->ss == rhs->ss) return TRUE;
	return FALSE;
}

BOOL time_GT(time*lhs, time*rhs){
	if(lhs->hh > rhs->hh) return TRUE;
	if(lhs->hh == rhs->hh && lhs->mm > rhs->mm) return TRUE;
	if(lhs->hh == rhs->hh && lhs->mm == rhs->mm && lhs->ss > rhs->ss) return TRUE;
	return FALSE;
}

BOOL time_LT(time*lhs, time*rhs){
	if(lhs->hh < rhs->hh) return TRUE;
	if(lhs->hh == rhs->hh && lhs->mm < rhs->mm) return TRUE;
	if(lhs->hh == rhs->hh && lhs->mm == rhs->mm && lhs->ss < rhs->ss) return TRUE;
	return FALSE;
}

BOOL time_start_end(schedule *sch, time *s){
	BOOL On = FALSE;
	 // activate UV Lamp by schedule
	time end = time_ADD(&sch->start, &sch->dur);
	 if (time_LT(&sch->start, s) && time_GT(&end, s)){
		 On = TRUE;
	 }
	 return On;
}

BOOL time_start_end_arr(schedule* sch,time *s, int amt){
	BOOL On = FALSE;
	int i;
	for(i = 0; i < amt ; i++)	{
		On += time_start_end(&sch[i],s);
	}
	if (On) return TRUE;
	return FALSE;
}

#endif /* UTILS_H_ */
