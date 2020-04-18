/* Directive -----------------------------------------------------------------*/
//#define SD_V1
//#define SD_V2
//#define SD_V3
#define SD_V4
//#define SD_V5

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

#ifdef SD_V2
/* SD_v2 part */
#include "SDIOBlockDevice.h"
//#include "FATFileSystem.h"
#include "LittleFileSystem.h"
#endif

#ifdef SD_V3
/* SD_v3 part */
//#include "stm32f769i_discovery_sd.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#endif

#ifdef SD_V4
#include "FATFileSystem.h"
#include "SDBlockDeviceDISCOF769NI.h"
#endif

#ifdef SD_V5
#include "FATFileSystem.h"
#include "SDBlockDevice.h"
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
    
#ifdef SD_V2
/* SD_v2 part */
SDIOBlockDevice bd;
//FATFileSystem fs("fs", &bd);
LittleFileSystem fs("fs",&bd);
#endif

#ifdef SD_V3
FATFS SDFatFs;  /* File system object for SD card logical drive */
char SDPath[4]; /* SD card logical drive path */
#endif

#ifdef SD_V4
SDBlockDeviceDISCOF769NI bd;
FATFileSystem fs("leka");
#endif

#ifdef SD_V5
/*
    SDMMC2_CLK = PD6 (B11) -> SCK
    SDMMC2_CMD = PD7 (A11) -> MOSI
    SDMMC2_D0 = PG9 (D9) -> MISO
    SDMMC2_D1 = PG10 (C8) -> RSV
    SDMMC2_D2 = PB3 (A10) -> GND
    SDMMC2_D3 = PB4 (A9) -> CSn
 */

SDBlockDevice bd(PD_7, PG_9, PD_6, PB_4);
#define BUFFER_MAX_LEN 10
FATFileSystem fs("leka");
#endif

/* Private function prototypes -----------------------------------------------*/
/* LCD part */
static void LCD_BriefDisplay(void);

/* Private functions ---------------------------------------------------------*/
int main(void) {
    debug_if(DBG, "\n\nRESET\r\n\n");

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
    
#ifdef SD_V2
    /* SD_v2 part */
    //sd.init();
    //fflush(stdout);
    //int err = fs.mount(bd);
    //debug_if(DBG, "%s\n", (err ? "Fail :(" : "OK"));
#endif

#ifdef SD_V3
    if(FATFS_LinkDriver(&SD_Driver, SDPath)==0)
    {
        if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0)==FR_OK)
        {
            debug_if(DBG, "Test SD V3\r\n"); //Display even there is no SD card
        }
    }
#endif
    
#ifdef SD_V4
//    bd.init();
//    bd_size_t s = bd.size();
//    debug_if(DBG, "Size: %llu\r\n", s);
    
    int err;
    fflush(stdout);
//    err = fs.reformat(&bd, 0);
//    debug_if(DBG, "Reformat...%s\r\n", (err ? "Fail :(" : "OK"));
//    fflush(stdout);
    err = fs.mount(&bd);
    debug_if(DBG, "Mounting... %s\r\n", (err ? "Fail :(" : "OK"));
    fflush(stdout);
    FILE *f = fopen("/leka/image.jpg", "r");
    debug_if(DBG, "Opening JPG... %s\r\n", (!f ? "Fail :(" : "OK"));
    fflush(stdout);
    f = fopen("/leka/numbers.txt", "w+");
    debug_if(DBG, "Opening Numbers (writing)... %s\r\n", (!f ? "Fail :(" : "OK"));
    if (err)
    {
        return 0;
    }
    for (int i =0 ; i<9 ; i++)
    {
        fflush(stdout);
        fprintf(f,"Test %d\r\n", i);
    }
    err = fprintf(f,"End Test");
    debug_if(DBG, "Opening Numbers (wrote)... %s\r\n", (err < 0 ? "Fail :(" : "OK"));
    fflush(stdout);
    err = fclose(f);
    debug_if(DBG, "Closing File (wrote)... %s\r\n", (err < 0 ? "Fail :(" : "OK"));
    
    bd_size_t s = bd.size();
    debug_if(DBG, "Size: %llu\r\n", s);
    
    fflush(stdout);
    DIR* dir = opendir("/leka/");
    debug_if(DBG, "Opening Directory (writing)... %s\r\n", (!dir ? "Fail :(" : "OK"));
    debug_if(DBG, "Printing all filenames:\r\n");

    while (true) {
        struct dirent *e = readdir(dir);
        if (!e) {
            break;
        }
        printf("    %s\r\n", e->d_name);
    }
//    struct dirent* de;
//     while((de = readdir (dir)) != NULL) {
//         printf("  %s\r\n", &(de->d_name));
//     }

    fflush(stdout);
    closedir(dir);
    
    err = fs.unmount();
    debug_if(DBG, "Unmounting... %s\r\n", (err ? "Fail :(" : "OK"));
#endif

#ifdef SD_V5
    int err;
    fflush(stdout);
    err = fs.mount(&bd);
    debug_if(DBG, "Mounting... %s\r\n", (err ? "Fail :(" : "OK"));
    fflush(stdout);
    FILE *f = fopen("/leka/image.jpg", "r");
    debug_if(DBG, "Opening JPG... %s\r\n", (!f ? "Fail :(" : "OK"));
    fclose(f);
#endif
    
    /* DEBUG part */
    while (true) {
        debug_if(DBG, "End of program\r\n");
        wait_us(10000000);
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
