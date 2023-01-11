/*
 * timer.h
 *
 * Created: 07.12.2022 16:10:17
 *  Author: sshmykov
 */ 


#ifndef TIMER_H_
#define TIMER_H_

void int0_init(void);
void int1_init(void);

void start_timer1(void);
void stop_timer1(void);
void reset_timer1(void);

#endif /* TIMER_H_ */