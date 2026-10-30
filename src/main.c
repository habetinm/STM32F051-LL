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
}

void Timer_Loop(void) {
    for (int i = 0; i < MAX_TIMERS; i++) {
	if ((milliseconds - Timers[i].time) >= Timers[i].interval) {
	    Timers[i].time = milliseconds;
	    if (Timers[i].timer_cb != 0) {
	        Timers[i].timer_cb();
	    }
	}
    }
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

    /* No pull-up or pull-down */
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_9, LL_GPIO_PULL_NO);

    Timer_Init();
    Timer_Set(TIMER_0, 300, Led3_Toggle);
    Timer_Set(TIMER_1, 400, Led4_Toggle);

    while (1) {
	Timer_Loop();
    }
}
