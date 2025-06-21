#include "stm32f1xx.h"
#include "main.h"

#define LORA_JAMMER_PIN GPIO_PIN_0
#define WIFI_JAMMER_PIN GPIO_PIN_1
#define GSM_JAMMER_PIN  GPIO_PIN_2

void SystemClock_Config(void);
void GPIO_Config(void);
void Delay(uint32_t);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Config();

    while (1)
    {
        HAL_GPIO_TogglePin(GPIOA, LORA_JAMMER_PIN);  // Lora ON/OFF
        HAL_Delay(2000);
        HAL_GPIO_TogglePin(GPIOA, WIFI_JAMMER_PIN);  // WiFi ON/OFF
        HAL_Delay(2000);
        HAL_GPIO_TogglePin(GPIOA, GSM_JAMMER_PIN);   // GSM ON/OFF
        HAL_Delay(2000);
    }
}

void GPIO_Config(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LORA_JAMMER_PIN | WIFI_JAMMER_PIN | GSM_JAMMER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}