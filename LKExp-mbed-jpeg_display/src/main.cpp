/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
#include "stm32f769i_discovery_lcd.h"

/* Exported constants --------------------------------------------------------*/
#define LCD_FRAME_BUFFER         0xC0000000

/* Private variables ---------------------------------------------------------*/
static uint32_t LCD_X_Size = 0;

/* Private function prototypes -----------------------------------------------*/
static void LCD_BriefDisplay(void);

/* Private functions ---------------------------------------------------------*/
Serial pc(USBTX, USBRX);

int main(void) {
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER);
    BSP_LCD_SelectLayer(0);
    LCD_X_Size = BSP_LCD_GetXSize();
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    LCD_BriefDisplay();
    while (true) {
        pc.printf("Screen width is %d pixels\r\n", LCD_X_Size);
        wait(1);
    }
}

static void LCD_BriefDisplay(void)
{
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    BSP_LCD_FillRect(0, 0, LCD_X_Size, 112);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(0, LINE(2), (uint8_t *)"Trying some display", CENTER_MODE);
}
