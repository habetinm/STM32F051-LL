#include "stm32f0xx.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_gpio.h"

volatile uint32_t milliseconds = 0;

void SysTick_Handler(void)
{
    milliseconds++;
}

void Delay_ms(uint32_t delay)
{
    uint32_t start = milliseconds;

    while ((milliseconds - start) < delay)
    {
        /* Wait */
    }
}

int main(void)
{
    /* Initialize SysTick for 1 ms interrupt */
    SysTick_Config(SystemCoreClock / 1000);
    
	/* Enable clock for GPIOC */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

    /* Configure PC9 as output */
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);

    /* Push-pull output */
    LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_9,
                             LL_GPIO_OUTPUT_PUSHPULL);

    /* No pull-up or pull-down */
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_9, LL_GPIO_PULL_NO);

    while (1)
    {
        LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_9);

	Delay_ms(50);
    }
}
