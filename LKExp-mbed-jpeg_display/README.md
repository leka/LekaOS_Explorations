# LKExp-mbed-jpeg_display

## Goal

Display a JPEG image on the LCD using Jpeg Codec, DMA2D, LTDC and DSI if necessary.

## Ressources

[st\_jpeg\_animation_example](https://github.com/leka/LekaOS_Explorations/tree/yann-st_jpeg_animation_example)

Mount SD (BSP) : [BD\_SD\_DISCO\_F769NI](https://os.mbed.com/users/kenjiArai/code/BD_SD_DISCO_F769NI/)

Mount SD (SDBlockDevice customized) : [sdio-driver](https://github.com/JojoS62/Test-lvgl/tree/master/components/sdio-driver) **[Does not work]**


## Status

In progress...
Next step : Display an image in easiest way (from flash instead of SD?, a bitmap file instead of jpeg?)

## Warning

Do not use [SDBlockDevice](https://os.mbed.com/docs/mbed-os/v5.15/apis/sdblockdevice.html), it required an SD host with SPI interface. In case of Disco-769, it is a SDMMC interface.