#include "ds18b20.h"
void ds18b20_init()
{
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_CFGCMP);
  
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	

}

void DQININT()//ÊäÈëÅäÖÃ
{
  rcu_periph_clock_enable(RCU_GPIOB);
	gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
	
}

void DQOUTINT()//ÊäÈëÅäÖÃ
{
  rcu_periph_clock_enable(RCU_GPIOB);
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
}
void ds18b20init()
{
   DQOUTINT();
   ds18b20_dq_L;
   delay_10us(48);
   ds18b20_dq_H;
   delay_10us(48);
}

void ds18b20wr(uint8_t dat)
 {
   uint8_t i=0;
   DQOUTINT();

   for(i=0;i<8;i++)
   {
   	  ds18b20_dq_L;
	  delay_10us(2);
	  if((dat&0x01)==1)
	  {
	  	  ds18b20_dq_H;
	  }
	  else
   	  {
	  	  ds18b20_dq_L;
	  
	  }
	  delay_10us(6);
	  ds18b20_dq_H;
	  dat>>=1;
   }
 }

uint8_t da18b20rd()
{
  uint8_t i=0,value=0;
  for(i=0;i<8;i++)
  {
    value>>=1;
	DQOUTINT();
	ds18b20_dq_L;
	delay_10us(1);
	ds18b20_dq_H;
	delay_10us(1);
	DQININT();
	if(gpio_input_bit_get(GPIO_ds18b20,dq)==SET)
	{
	  value|=0x80;
	}
  	  delay_10us(5);
  }
	return value;

}

double readtemp()
{
   uint8_t a,b;
   uint16_t temp;
   double value;
   ds18b20init();
   ds18b20wr(0xcc);
   ds18b20wr(0x44);
   delay_1ms(100);

   ds18b20init();
   ds18b20wr(0xcc);
   ds18b20wr(0xbe);
   a=da18b20rd();
   b=da18b20rd();
   temp=b;
   temp=(temp<<8)+a;
   if((temp&0xf800)==0xf800)
   {
			temp=(~temp)+1;
			value=temp*(-0.0625);
   }
	 else
	 {
	   value=temp*0.0625;
	 }
	 return value;

}






