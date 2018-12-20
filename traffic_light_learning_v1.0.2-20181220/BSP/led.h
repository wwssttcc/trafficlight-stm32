#ifndef _LED_H_
#define _LED_H_

#define LED1 		GPIOE , GPIO_Pin_9
#define LED2 		GPIOE , GPIO_Pin_14
#define LED3 		GPIOE , GPIO_Pin_15

#define LED1_ON 		GPIO_ResetBits(GPIOE , GPIO_Pin_9)
#define LED2_ON 		GPIO_ResetBits(GPIOE , GPIO_Pin_14)
#define LED3_ON 		GPIO_ResetBits(GPIOE , GPIO_Pin_15)



#define LED1_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_9)
#define LED2_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_14)
#define LED3_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_15)

#define TRAFFIC_LED1_ON 		GPIO_ResetBits(GPIOA , GPIO_Pin_0)
#define TRAFFIC_LED2_ON 		GPIO_ResetBits(GPIOA , GPIO_Pin_3)
#define TRAFFIC_LED3_ON 		GPIO_ResetBits(GPIOA , GPIO_Pin_5)

#define TRAFFIC_LED1_OFF 		GPIO_SetBits(GPIOA , GPIO_Pin_0)
#define TRAFFIC_LED2_OFF 		GPIO_SetBits(GPIOA , GPIO_Pin_3)
#define TRAFFIC_LED3_OFF 		GPIO_SetBits(GPIOA , GPIO_Pin_5)
void LED_Configuration(void);

#endif
