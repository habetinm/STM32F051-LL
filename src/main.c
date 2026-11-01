#include "stm32f0xx.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_gpio.h"
#include <string.h>
#include <stdio.h>

#define TIMER_0     0
#define TIMER_1     1
#define TIMER_2     2
#define TIMER_3     3
#define TIMER_4     4
#define TIMER_5     5
#define TIMER_6     6
#define TIMER_7     7
#define TIMER_8     8
#define TIMER_9     9
#define MAX_TIMERS 10

typedef void (*Callback)(void);

typedef struct {
    uint8_t used;
    uint8_t running;
    uint32_t time;
    uint32_t interval;
    Callback timer_cb;
} Timer;

static Timer Timers[MAX_TIMERS];

volatile uint32_t milliseconds = 0;

void SysTick_Handler(void) {
    milliseconds++;
}

void Timer_Init(void) {
    memset(Timers, 0, sizeof(Timers));
}

void Timer_Set(uint8_t timer_index, uint32_t interval, Callback cb) {
    Timers[timer_index].time = milliseconds;
    Timers[timer_index].interval = interval; 
    Timers[timer_index].timer_cb = cb;
    Timers[timer_index].used = 1;
    //Timers[timer_index].running = 1;
}

void Timer_Loop(void) {
    for (int i = 0; i < MAX_TIMERS; i++) {
	if (Timers[i].used) {
	    if (Timers[i].running) {	
	        if ((milliseconds - Timers[i].time) >= Timers[i].interval) {
	            Timers[i].time = milliseconds;
	            if (Timers[i].timer_cb != 0) {
	                Timers[i].timer_cb();
	            }
	        }
	    }
	}
    }
}

void Timer_Pause(uint8_t timer_index) {
    Timers[timer_index].running = 0;
}

void Timer_Resume(uint8_t timer_index) {
    Timers[timer_index].running = 1;
}

void Timer_Start(uint8_t timer_index) {
    Timers[timer_index].running = 1;
}

uint8_t Timer_Paused(uint8_t timer_index) {
    return ~Timers[timer_index].running;
}

void Delay_ms(uint32_t delay) {
    uint32_t start = milliseconds;

    while ((milliseconds - start) < delay) {
        /* Wait */
    }
}

void Led3_Toggle(void) {
    LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_9);
}

void Led4_Toggle(void) {
    LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_8);
}

void Led3_ChangePeriod(void) {
    uint32_t new_period = 0;
    static uint8_t period_mul = 0;
    period_mul += 1;
    period_mul %= 2;
    new_period = 100 + period_mul * 100;
    Timer_Set(TIMER_0, new_period, Led3_Toggle);
}

void Led4_SmoothChangePeriod(void) {
    uint32_t new_period = 0;
    static uint8_t counter = 0;
    counter += 1;
    counter %= 4;
    new_period = 1000 - counter * 300;
    Timer_Set(TIMER_1, new_period, Led4_Toggle);
}

int main(void) {
    /* Initialize SysTick for 1 ms interrupt */
    SysTick_Config(SystemCoreClock / 1000);
    
	/* Enable clock for GPIOC */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

    /* Configure PC9 as output */
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);

    /* Push-pull output */
    LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_PUSHPULL);

    /* No pull-up or pull-down */
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_9, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_8, LL_GPIO_PULL_NO);

    LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_9);
    LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_8);

    /* Enable clock for GPIOA */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    /* Configure PA0 as input */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT);

    Timer_Init();
    Timer_Set(TIMER_0,  100, Led3_Toggle);
    Timer_Set(TIMER_1, 1000, Led4_Toggle);
    Timer_Set(TIMER_2, 2000, Led3_ChangePeriod);
    Timer_Set(TIMER_3, 5000, Led4_SmoothChangePeriod);

    Timer_Start(TIMER_0);
    Timer_Start(TIMER_1);
    Timer_Start(TIMER_2);
    Timer_Start(TIMER_3);

    while (1) {
	static uint8_t user_button_last_state = 0;
	Timer_Loop();
	if (!LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0)) {
	    if (!user_button_last_state) {
	        user_button_last_state = 1;
	        if (Timer_Paused(TIMER_0)) {
                    Timer_Resume(TIMER_0);
	        }
	    }
	} else {
	    if (user_button_last_state) {
	        user_button_last_state = 0;
	        Timer_Pause(TIMER_0);
	    }
	} 
    }
}
