# LKExp - JPEG Display with Mbed

 - Etapes d'initialisation de l'écran LCD : 
    - Reset (réinitialise les pins GPIO utilisés)
    - MspInit (Active un certain nombre de fonctionnalités : LTDC clock, DMA2D clock, reset du DSI host, interruptions pour LTDC, DMA2D et DSI ...)
    - Configurer la DSI clock
    - configurer le DSI (NumberofLanes, TXEscapeCkDiv)
    - Initialiser le DSI avec les configurations
    - Configurer le DSI Video
        1. Paramètres de synchronisation (start active time, back porch, active time, front porch) pour horizontal et vertical (prendre les valeurs du driver OTM8009a pour stm32f7xx)
        2. Color coding (DSI_RGB888)
        3. Polarité (active high)
        3. Mode (video burst)
        4. Low Power enable/disable
    - Initialiser le DSI Video avec la configuration
    - Configurer et initialiser la clock du LCD (HAL_RCCEx_PeriphCLKConfig)
    - Configurer le LTDC ( Backcolor, polarité, synchronisation → utiliser `HAL_LTDC_StructInitFromVideoConfig(&hltdc, &hdsivideo)` )
    - Initialiser le LTDC
    - Start le DSI
    - Initialiser la SDRAM
    - Initialiser le driver OTM8009a

 - Pour afficher un pixel à l'écran : écrire directement la couleur dans le frame buffer de la layer concernée à la bonne adresse et au bon format (ARGB8888 dans notre cas)

 - Pour remplir une surface sur l'écran : utilisation du DMA2D en mode R2M (register to memory) qui se charge du frame buffer

 - Pour afficher une image depuis un tableau de données (couleur des pixels en ARGB888) : utilisation du DMA2D en mode M2M (memory to memory) et des deux layers pour effectuer le mélange des canaux alpha avec `HAL_DMA2D_BlendingStart` au lieu de `HAL_DMA2D_Start`

 - Pour afficher une image jpeg chargée depuis la carte SD : utilisation du JPEG decoder puis copie du buffer avec le DMA2D

 - Pour afficher une vidéo : Comming soon ™