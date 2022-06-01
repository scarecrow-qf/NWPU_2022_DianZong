#include "key.h"

uint16_t key_value_time1 = 0;
uint16_t key_value_time2 = 0;
uint8_t key_value_flag1 = 0;
uint8_t switch_key = 0;

uint8_t KEY_Scan()
{
	if((KEY0 == 0)|(KEY1 == 0))
	{
		key_value_flag1 = 1;
		if((KEY0 == 0)|(KEY1 == 0))
		{
			key_value_time1++;
			if(KEY0==0)
				switch_key=1;
			else if(KEY1==0)
				switch_key=2;
			else
				switch_key=0;
		}
	}
	else 
	{  // 松开按键
		key_value_flag1 = 0;
		if((key_value_time1 >= 100)&&(switch_key==1)) //5ms一次中断
		{ 
			key_value_time1 = 0;
			return 3;
		}
		else if(key_value_time1>=2)
		{
			key_value_time1 = 0;
			if(switch_key==1)
				return 1;
			else if(switch_key==2)
				return 2;
			else
				return 0;
		}
		else 
		{
			key_value_time1 = 0;
			return 0;
		}
	}
		
	
 	return 0;// 无按键按下
}

