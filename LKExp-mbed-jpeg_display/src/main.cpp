/* Directive -----------------------------------------------------------------*/
//#define SD_V1
#define SD_V2

/* Includes ------------------------------------------------------------------*/
#include "mbed.h"

/* LCD part */
#include "stm32f769i_discovery_lcd.h"

#ifdef SD_V1
/* SD_v1 part */
//#include "BlockDevice.h"
//#include "platform/PlatformMutex.h"
#include "stm32f769i_discovery_sd.h"
#endif


/* Exported constants --------------------------------------------------------*/
/* DEBUG part */
#define DBG 1
/* LCD part */
#define LCD_FRAME_BUFFER         0xC0000000

/* Private variables ---------------------------------------------------------*/
/* DEBUG part */
Serial pc(USBTX, USBRX);
/* LCD part */
static uint32_t LCD_X_Size = 0;

#ifdef SD_V1
/* SD_v1 part */
BSP_SD_CardInfo _current_card_info;
uint8_t _sd_state;
//PlatformMutex _mutex;
#endif

/* Private function prototypes -----------------------------------------------*/
/* LCD part */
static void LCD_BriefDisplay(void);

/* Private functions ---------------------------------------------------------*/
int main(void) {
    /* LCD part */
    //    BSP_LCD_Init();
    //    BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER);
    //    BSP_LCD_SelectLayer(0);
    //    LCD_X_Size = BSP_LCD_GetXSize();
    //    BSP_LCD_Clear(LCD_COLOR_WHITE);
    //    LCD_BriefDisplay();
    
#ifdef SD_V1
    /* SD_v1 part */
    //_mutex.lock();
    _sd_state = BSP_SD_Init();
    BSP_SD_GetCardInfo(&_current_card_info);
#endif

    /* DEBUG part */
    printf("\n\nRESET\r\n\n");
    while (true) {
        debug_if(DBG, "test debug\r\n");
        wait_us(1000000);
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
