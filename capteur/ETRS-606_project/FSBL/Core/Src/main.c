/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "stdio.h"

#include "lsm6dso_reg.h"
#include "lis2mdl_reg.h"
#include "hts221_reg.h"

/* Private define ------------------------------------------------------------*/
#define LSM6DSO_ADDR   (0x6A << 1)
#define LIS2MDL_ADDR   (0x1E << 1)
#define HTS221_ADDR    (0x5F << 1)

/* Private variables ---------------------------------------------------------*/
COM_InitTypeDef BspCOMInit;
__IO uint32_t BspButtonState = BUTTON_RELEASED;

I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

stmdev_ctx_t dev_ctx;
stmdev_ctx_t mag_ctx;
stmdev_ctx_t hum_ctx;

uint16_t lsm6dso_addr = LSM6DSO_ADDR;
uint16_t lis2mdl_addr = LIS2MDL_ADDR;
uint16_t hts221_addr  = HTS221_ADDR;

int16_t acc_raw[3];
int16_t mag_raw[3];
int16_t temp_raw;
int16_t hum_raw;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);

int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);

/* USER CODE BEGIN 0 */

float hts221_from_lsb_to_celsius_custom(int16_t lsb)
{
    float t0_degc, t1_degc, t0_out, t1_out;

    hts221_temp_deg_point_0_get(&hum_ctx, &t0_degc);
    hts221_temp_deg_point_1_get(&hum_ctx, &t1_degc);
    hts221_temp_adc_point_0_get(&hum_ctx, &t0_out);
    hts221_temp_adc_point_1_get(&hum_ctx, &t1_out);

    if ((t1_out - t0_out) == 0.0f)
        return 0.0f;

    return t0_degc + ((float)lsb - t0_out) *
           (t1_degc - t0_degc) / (t1_out - t0_out);
}

float hts221_from_lsb_to_humidity_custom(int16_t lsb)
{
    float h0_rh, h1_rh, h0_out, h1_out;

    hts221_hum_rh_point_0_get(&hum_ctx, &h0_rh);
    hts221_hum_rh_point_1_get(&hum_ctx, &h1_rh);
    hts221_hum_adc_point_0_get(&hum_ctx, &h0_out);
    hts221_hum_adc_point_1_get(&hum_ctx, &h1_out);

    if ((h1_out - h0_out) == 0.0f)
        return 0.0f;

    float humidity = h0_rh + ((float)lsb - h0_out) *
                     (h1_rh - h0_rh) / (h1_out - h0_out);

    if (humidity > 100.0f) humidity = 100.0f;
    if (humidity < 0.0f)   humidity = 0.0f;

    return humidity;
}

/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();

    BSP_LED_Init(LED_BLUE);
    BSP_LED_Init(LED_RED);
    BSP_LED_Init(LED_GREEN);
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

    BspCOMInit.BaudRate   = 115200;
    BspCOMInit.WordLength = COM_WORDLENGTH_8B;
    BspCOMInit.StopBits   = COM_STOPBITS_1;
    BspCOMInit.Parity     = COM_PARITY_NONE;
    BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;

    if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
    {
        Error_Handler();
    }

    printf("\r\nWelcome to STM32 world !\r\n");
    printf("Boot project is running...\r\n");

    dev_ctx.read_reg  = platform_read;
    dev_ctx.write_reg = platform_write;
    dev_ctx.handle    = &lsm6dso_addr;

    mag_ctx.read_reg  = platform_read;
    mag_ctx.write_reg = platform_write;
    mag_ctx.handle    = &lis2mdl_addr;

    hum_ctx.read_reg  = platform_read;
    hum_ctx.write_reg = platform_write;
    hum_ctx.handle    = &hts221_addr;

    uint8_t whoamI = 0;

    printf("\r\n--- SCAN DES CAPTEURS I2C ---\r\n");

    lsm6dso_device_id_get(&dev_ctx, &whoamI);
    if (whoamI == 0x6C)
        printf("[OK] LSM6DSO detecte ID = 0x%02X\r\n", whoamI);
    else
        printf("[ECHEC] LSM6DSO introuvable, lu = 0x%02X\r\n", whoamI);

    lis2mdl_device_id_get(&mag_ctx, &whoamI);
    if (whoamI == 0x40)
        printf("[OK] LIS2MDL detecte ID = 0x%02X\r\n", whoamI);
    else
        printf("[ECHEC] LIS2MDL introuvable, lu = 0x%02X\r\n", whoamI);

    hts221_device_id_get(&hum_ctx, &whoamI);
    if (whoamI == 0xBC)
        printf("[OK] HTS221 detecte ID = 0x%02X\r\n", whoamI);
    else
        printf("[ECHEC] HTS221 introuvable, lu = 0x%02X\r\n", whoamI);

    printf("-----------------------------\r\n");

    /* Initialisation LSM6DSO */
    lsm6dso_reset_set(&dev_ctx, PROPERTY_ENABLE);
    HAL_Delay(100);
    lsm6dso_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
    lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_104Hz);
    lsm6dso_xl_full_scale_set(&dev_ctx, LSM6DSO_2g);

    /* Initialisation LIS2MDL */
    lis2mdl_reset_set(&mag_ctx, PROPERTY_ENABLE);
    HAL_Delay(100);
    lis2mdl_data_rate_set(&mag_ctx, LIS2MDL_ODR_10Hz);

    /* Initialisation HTS221 */
    hts221_power_on_set(&hum_ctx, PROPERTY_ENABLE);
    HAL_Delay(100);
    hts221_block_data_update_set(&hum_ctx, PROPERTY_ENABLE);
    hts221_data_rate_set(&hum_ctx, HTS221_ODR_1Hz);
    HAL_Delay(100);

    printf("System ready. Demarrage de la boucle principale.\r\n\r\n");

    while (1)
    {
        float ax = 0.0f, ay = 0.0f, az = 0.0f;
        float mx = 0.0f, my = 0.0f, mz = 0.0f;
        static float temp = 0.0f;
        static float hum  = 0.0f;

        hts221_status_reg_t status;

        /* Accelerometre */
        lsm6dso_acceleration_raw_get(&dev_ctx, acc_raw);

        ax = lsm6dso_from_fs2_to_mg(acc_raw[0]);
        ay = lsm6dso_from_fs2_to_mg(acc_raw[1]);
        az = lsm6dso_from_fs2_to_mg(acc_raw[2]);

        /* Magnetometre */
        lis2mdl_magnetic_raw_get(&mag_ctx, mag_raw);

        mx = lis2mdl_from_lsb_to_mgauss(mag_raw[0]);
        my = lis2mdl_from_lsb_to_mgauss(mag_raw[1]);
        mz = lis2mdl_from_lsb_to_mgauss(mag_raw[2]);

        /* Temperature + Humidite */
        hts221_status_get(&hum_ctx, &status);

        if (status.h_da)
        {
            hts221_humidity_raw_get(&hum_ctx, &hum_raw);
            hum = hts221_from_lsb_to_humidity_custom(hum_raw);
        }

        if (status.t_da)
        {
            hts221_temperature_raw_get(&hum_ctx, &temp_raw);
            temp = hts221_from_lsb_to_celsius_custom(temp_raw);
        }

        printf("ACC [mg] : X=%.2f Y=%.2f Z=%.2f\r\n", ax, ay, az);
        printf("MAG [mG] : X=%.2f Y=%.2f Z=%.2f\r\n", mx, my, mz);
        printf("HTS221 STATUS : h_da=%d t_da=%d | raw_h=%d raw_t=%d\r\n",
               status.h_da, status.t_da, hum_raw, temp_raw);
        printf("TEMP = %.2f C | HUM = %.2f %%\r\n", temp, hum);
        printf("-----------------------------\r\n");

        BSP_LED_On(LED_RED);
        BSP_LED_Off(LED_GREEN);
        BSP_LED_Off(LED_BLUE);
        HAL_Delay(300);

        BSP_LED_Off(LED_RED);
        BSP_LED_On(LED_GREEN);
        BSP_LED_Off(LED_BLUE);
        HAL_Delay(300);

        BSP_LED_Off(LED_RED);
        BSP_LED_Off(LED_GREEN);
        BSP_LED_On(LED_BLUE);
        HAL_Delay(300);

        BSP_LED_Off(LED_BLUE);

        HAL_Delay(1000);
    }
}

/**
  * @brief System Clock Configuration
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_RCC_GetClockConfig(&RCC_ClkInitStruct);

    if ((RCC_ClkInitStruct.CPUCLKSource == RCC_CPUCLKSOURCE_IC1) ||
        (RCC_ClkInitStruct.SYSCLKSource == RCC_SYSCLKSOURCE_IC2_IC6_IC11))
    {
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK;
        RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_HSI;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;

        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }
    }

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL1.PLLM = 4;
    RCC_OscInitStruct.PLL1.PLLN = 75;
    RCC_OscInitStruct.PLL1.PLLFractional = 0;
    RCC_OscInitStruct.PLL1.PLLP1 = 1;
    RCC_OscInitStruct.PLL1.PLLP2 = 1;
    RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_CPUCLK |
                                  RCC_CLOCKTYPE_HCLK |
                                  RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2 |
                                  RCC_CLOCKTYPE_PCLK5 |
                                  RCC_CLOCKTYPE_PCLK4;

    RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
    RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV1;

    RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    RCC_ClkInitStruct.IC1Selection.ClockDivider = 2;
    RCC_ClkInitStruct.IC2Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    RCC_ClkInitStruct.IC2Selection.ClockDivider = 3;
    RCC_ClkInitStruct.IC6Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    RCC_ClkInitStruct.IC6Selection.ClockDivider = 4;
    RCC_ClkInitStruct.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    RCC_ClkInitStruct.IC11Selection.ClockDivider = 3;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief I2C1 Initialization Function
  */
static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x10C0ECFF;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_10 | UCPD1_VSENSE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    uint16_t dev_addr = *(uint16_t *)handle;

    if (HAL_I2C_Mem_Read(&hi2c1,
                         dev_addr,
                         reg,
                         I2C_MEMADD_SIZE_8BIT,
                         bufp,
                         len,
                         1000) == HAL_OK)
    {
        return 0;
    }

    return -1;
}

int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len)
{
    uint16_t dev_addr = *(uint16_t *)handle;

    if (HAL_I2C_Mem_Write(&hi2c1,
                          dev_addr,
                          reg,
                          I2C_MEMADD_SIZE_8BIT,
                          (uint8_t *)bufp,
                          len,
                          1000) == HAL_OK)
    {
        return 0;
    }

    return -1;
}

/* USER CODE END 4 */

void BSP_PB_Callback(Button_TypeDef Button)
{
    if (Button == BUTTON_USER)
    {
        BspButtonState = BUTTON_PRESSED;
    }
}

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
        BSP_LED_On(LED_RED);
        HAL_Delay(200);
        BSP_LED_Off(LED_RED);
        HAL_Delay(200);
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
