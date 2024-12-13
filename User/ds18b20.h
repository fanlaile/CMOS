#ifndef _ds18b20_H
#define _ds18b20_H

#include "gd32e23x.h"
#include "systick.h"
#define dq GPIO_PIN_1 //PG11¹Ü½Å
#define GPIO_ds18b20 GPIOB
#define ds18b20_dq_H gpio_bit_write(GPIOB,GPIO_PIN_1,1)
#define ds18b20_dq_L gpio_bit_write(GPIOB,GPIO_PIN_1,0)

void ds18b20_init(void);
void DQININT(void);
void DQOUTINT(void);
void ds18b20init(void);
void ds18b20wr(uint8_t dat);
uint8_t da18b20rd(void);
double readtemp(void);


#endif

