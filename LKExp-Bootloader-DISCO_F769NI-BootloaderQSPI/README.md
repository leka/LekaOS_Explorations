# LKExp-Bootloader_with_QSPI

## Goal

From **LKExp-Bootloader-DISCO_F769NI-Bootloader_with_IAP** as a base, ask the user whether to store the current app in external flash or load a new one from it

## Result

⚠️ Reading/writing on the external flash not working: reading returns only 0x00 characters (check if it needs some initializations beforehand)
