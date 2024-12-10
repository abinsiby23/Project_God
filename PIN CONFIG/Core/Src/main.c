
#include "main.h"
#include "fatfs.h"
#include "lcd_driver.h"  // Include your LCD driver header
#include "cap_touch_driver.h"  // Include your capacitive touch driver header

// Global variables
char currentPath[256] = "0:/";  // Current directory path

// Function prototypes
void LCD_Init(void);
void CapacitiveTouch_Init(void);
void DisplayDirectory(const char* path);
void HandleTouchInput(void);

int main(void) {
    /* USER CODE BEGIN 1 */
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DCACHE1_Init();
    MX_FLASH_Init();
    MX_I2C1_Init();
    MX_I2C2_Init();
    MX_I2S1_Init();
    MX_IWDG_Init();
    MX_SDMMC1_MMC_Init();
    MX_SPI2_Init();
    MX_SPI3_Init();
    MX_UART4_Init();
    MX_USART3_UART_Init();
    MX_USB_HCD_Init();
    MX_WWDG_Init();
    MX_ICACHE_Init();

    /* USER CODE BEGIN 2 */
    LCD_Init();              // Initialize the LCD
    CapacitiveTouch_Init();  // Initialize capacitive touch
    DisplayDirectory(currentPath);  // Display root directory on the LCD
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        HandleTouchInput();  // Process user touch input for navigation
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

void DisplayDirectory(const char* path) {
    FATFS fs;
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int yPos = 30;  // Start Y position for listing

    LCD_Clear();  // Clear the screen
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_SetBackColor(LCD_COLOR_BLACK);
    LCD_DisplayStringAt(0, 0, (uint8_t*)path, CENTER_MODE);  // Display current path

    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        while (1) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;  // Break on error or end of directory

            if (fno.fattrib & AM_DIR) {
                LCD_DisplayStringAt(10, yPos, (uint8_t*)"[DIR] ", LEFT_MODE);
                LCD_DisplayStringAt(60, yPos, (uint8_t*)fno.fname, LEFT_MODE);
            } else {
                LCD_DisplayStringAt(10, yPos, (uint8_t*)"[FILE] ", LEFT_MODE);
                LCD_DisplayStringAt(60, yPos, (uint8_t*)fno.fname, LEFT_MODE);
            }
            yPos += 20;  // Move to the next line
        }
        f_closedir(&dir);
    } else {
        LCD_DisplayStringAt(0, 30, (uint8_t*)"Failed to open directory.", CENTER_MODE);
    }
}

void HandleTouchInput(void) {
    CapTouch_StateTypeDef touchState;
    CapacitiveTouch_GetState(&touchState);

    if (touchState.touchDetected) {
        uint16_t x = touchState.touchX[0];
        uint16_t y = touchState.touchY[0];

        if (y < 30) {  // Touch near the top could represent "Go Back"
            char* lastSlash = strrchr(currentPath, '/');
            if (lastSlash && lastSlash != currentPath) {
                *lastSlash = '\0';
                DisplayDirectory(currentPath);
            }
        } else {
            int selectedItem = (y - 30) / 20;
            FATFS fs;
            FRESULT res;
            DIR dir;
            FILINFO fno;
            res = f_opendir(&dir, currentPath);
            if (res == FR_OK) {
                int index = 0;
                while (1) {
                    res = f_readdir(&dir, &fno);
                    if (res != FR_OK || fno.fname[0] == 0) break;
                    if (index == selectedItem) {
                        if (fno.fattrib & AM_DIR) {
                            strcat(currentPath, "/");
                            strcat(currentPath, fno.fname);
                            DisplayDirectory(currentPath);
                        } else {
                            LCD_DisplayStringAt(0, 250, (uint8_t*)"File selected!", CENTER_MODE);
                        }
                        break;
                    }
                    index++;
                }
                f_closedir(&dir);
            }
        }
    }
}

void LCD_Init(void) {
    LCD_InitHardware();  // Replace with your LCD hardware initialization function
    LCD_SetOrientation(LCD_ORIENTATION_PORTRAIT);
    LCD_Clear();
}

void CapacitiveTouch_Init(void) {
    CapacitiveTouch_InitHardware();  // Replace with your touch controller initialization function
}
