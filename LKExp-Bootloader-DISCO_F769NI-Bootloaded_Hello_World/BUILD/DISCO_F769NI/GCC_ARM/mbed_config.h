/*
 * mbed SDK
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Automatically generated configuration file.
// DO NOT EDIT, content will be overwritten.

#ifndef __MBED_CONFIG_DATA__
#define __MBED_CONFIG_DATA__

// Configuration parameters
#define CLOCK_SOURCE                                                      USE_PLL_HSE_EXTC|USE_PLL_HSI                     // set by target:DISCO_F769NI
#define LPTICKER_DELAY_TICKS                                              0                                                // set by target:DISCO_F769NI
#define MBED_CONF_DRIVERS_QSPI_CSN                                        QSPI_FLASH1_CSN                                  // set by library:drivers
#define MBED_CONF_DRIVERS_QSPI_IO0                                        QSPI_FLASH1_IO0                                  // set by library:drivers
#define MBED_CONF_DRIVERS_QSPI_IO1                                        QSPI_FLASH1_IO1                                  // set by library:drivers
#define MBED_CONF_DRIVERS_QSPI_IO2                                        QSPI_FLASH1_IO2                                  // set by library:drivers
#define MBED_CONF_DRIVERS_QSPI_IO3                                        QSPI_FLASH1_IO3                                  // set by library:drivers
#define MBED_CONF_DRIVERS_QSPI_SCK                                        QSPI_FLASH1_SCK                                  // set by library:drivers
#define MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE                          256                                              // set by library:drivers
#define MBED_CONF_DRIVERS_UART_SERIAL_TXBUF_SIZE                          256                                              // set by library:drivers
#define MBED_CONF_EVENTS_PRESENT                                          1                                                // set by library:events
#define MBED_CONF_EVENTS_SHARED_DISPATCH_FROM_APPLICATION                 0                                                // set by library:events
#define MBED_CONF_EVENTS_SHARED_EVENTSIZE                                 768                                              // set by library:events
#define MBED_CONF_EVENTS_SHARED_HIGHPRIO_EVENTSIZE                        256                                              // set by library:events
#define MBED_CONF_EVENTS_SHARED_HIGHPRIO_STACKSIZE                        1024                                             // set by library:events
#define MBED_CONF_EVENTS_SHARED_STACKSIZE                                 2048                                             // set by library:events
#define MBED_CONF_EVENTS_USE_LOWPOWER_TIMER_TICKER                        0                                                // set by library:events
#define MBED_CONF_FAT_CHAN_FFS_DBG                                        0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_CODE_PAGE                                   437                                              // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_EXFAT                                    0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_HEAPBUF                                  1                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_LOCK                                     0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_MINIMIZE                                 0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_NOFSINFO                                 0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_NORTC                                    0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_READONLY                                 0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_REENTRANT                                0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_RPATH                                    1                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_TIMEOUT                                  1000                                             // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_FS_TINY                                     1                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_LFN_BUF                                     255                                              // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_LFN_UNICODE                                 0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_MAX_LFN                                     255                                              // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_MAX_SS                                      4096                                             // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_MIN_SS                                      512                                              // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_MULTI_PARTITION                             0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_NORTC_MDAY                                  1                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_NORTC_MON                                   1                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_NORTC_YEAR                                  2017                                             // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_SFN_BUF                                     12                                               // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_STRF_ENCODE                                 3                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_STR_VOLUME_ID                               0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_SYNC_T                                      HANDLE                                           // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_USE_CHMOD                                   0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_USE_EXPAND                                  0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_USE_FASTSEEK                                0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_USE_FIND                                    0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_USE_FORWARD                                 0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_USE_LABEL                                   0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_USE_LFN                                     3                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_USE_MKFS                                    1                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_USE_STRFUNC                                 0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_USE_TRIM                                    1                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_VOLUMES                                     4                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FF_VOLUME_STRS                                 "RAM","NAND","CF","SD","SD2","USB","USB2","USB3" // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FLUSH_ON_NEW_CLUSTER                           0                                                // set by library:fat_chan
#define MBED_CONF_FAT_CHAN_FLUSH_ON_NEW_SECTOR                            1                                                // set by library:fat_chan
#define MBED_CONF_FILESYSTEM_PRESENT                                      1                                                // set by library:filesystem
#define MBED_CONF_PLATFORM_CALLBACK_COMPARABLE                            1                                                // set by library:platform
#define MBED_CONF_PLATFORM_CALLBACK_NONTRIVIAL                            0                                                // set by library:platform
#define MBED_CONF_PLATFORM_CRASH_CAPTURE_ENABLED                          0                                                // set by library:platform
#define MBED_CONF_PLATFORM_CTHUNK_COUNT_MAX                               8                                                // set by library:platform
#define MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE                       9600                                             // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_ALL_THREADS_INFO                         0                                                // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_FILENAME_CAPTURE_ENABLED                 0                                                // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_HIST_ENABLED                             0                                                // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_HIST_SIZE                                4                                                // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_REBOOT_MAX                               1                                                // set by library:platform
#define MBED_CONF_PLATFORM_FATAL_ERROR_AUTO_REBOOT_ENABLED                0                                                // set by library:platform
#define MBED_CONF_PLATFORM_MAX_ERROR_FILENAME_LEN                         16                                               // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_ENABLE_64_BIT                   1                                                // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_ENABLE_FLOATING_POINT           0                                                // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_SET_FLOATING_POINT_MAX_DECIMALS 6                                                // set by library:platform
#define MBED_CONF_PLATFORM_POLL_USE_LOWPOWER_TIMER                        0                                                // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_BAUD_RATE                                9600                                             // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_BUFFERED_SERIAL                          0                                                // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_CONVERT_NEWLINES                         1                                                // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_CONVERT_TTY_NEWLINES                     1                                                // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_FLUSH_AT_EXIT                            1                                                // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_MINIMAL_CONSOLE_ONLY                     0                                                // set by library:platform
#define MBED_CONF_PLATFORM_USE_MPU                                        1                                                // set by library:platform
#define MBED_CONF_RTOS_API_PRESENT                                        1                                                // set by library:rtos-api
#define MBED_CONF_RTOS_EVFLAGS_NUM                                        0                                                // set by library:rtos
#define MBED_CONF_RTOS_IDLE_THREAD_STACK_SIZE                             512                                              // set by library:rtos
#define MBED_CONF_RTOS_IDLE_THREAD_STACK_SIZE_DEBUG_EXTRA                 128                                              // set by library:rtos[STM]
#define MBED_CONF_RTOS_IDLE_THREAD_STACK_SIZE_TICKLESS_EXTRA              256                                              // set by library:rtos
#define MBED_CONF_RTOS_MAIN_THREAD_STACK_SIZE                             4096                                             // set by library:rtos
#define MBED_CONF_RTOS_MSGQUEUE_DATA_SIZE                                 0                                                // set by library:rtos
#define MBED_CONF_RTOS_MSGQUEUE_NUM                                       0                                                // set by library:rtos
#define MBED_CONF_RTOS_MUTEX_NUM                                          0                                                // set by library:rtos
#define MBED_CONF_RTOS_PRESENT                                            1                                                // set by library:rtos
#define MBED_CONF_RTOS_SEMAPHORE_NUM                                      0                                                // set by library:rtos
#define MBED_CONF_RTOS_THREAD_NUM                                         0                                                // set by library:rtos
#define MBED_CONF_RTOS_THREAD_STACK_SIZE                                  4096                                             // set by library:rtos
#define MBED_CONF_RTOS_THREAD_USER_STACK_SIZE                             0                                                // set by library:rtos
#define MBED_CONF_RTOS_TIMER_NUM                                          0                                                // set by library:rtos
#define MBED_CONF_RTOS_TIMER_THREAD_STACK_SIZE                            768                                              // set by library:rtos
#define MBED_CONF_STORAGE_DEFAULT_KV                                      kv                                               // set by library:storage
#define MBED_CONF_STORAGE_FILESYSTEM_BLOCKDEVICE                          default                                          // set by library:storage_filesystem
#define MBED_CONF_STORAGE_FILESYSTEM_EXTERNAL_BASE_ADDRESS                0                                                // set by library:storage_filesystem
#define MBED_CONF_STORAGE_FILESYSTEM_EXTERNAL_SIZE                        0                                                // set by library:storage_filesystem
#define MBED_CONF_STORAGE_FILESYSTEM_FILESYSTEM                           default                                          // set by library:storage_filesystem
#define MBED_CONF_STORAGE_FILESYSTEM_FOLDER_PATH                          kvstore                                          // set by library:storage_filesystem
#define MBED_CONF_STORAGE_FILESYSTEM_INTERNAL_BASE_ADDRESS                0                                                // set by library:storage_filesystem
#define MBED_CONF_STORAGE_FILESYSTEM_MOUNT_POINT                          kv                                               // set by library:storage_filesystem
#define MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_BLOCKDEVICE                   default                                          // set by library:storage_filesystem_no_rbp
#define MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_EXTERNAL_BASE_ADDRESS         0                                                // set by library:storage_filesystem_no_rbp
#define MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_EXTERNAL_SIZE                 0                                                // set by library:storage_filesystem_no_rbp
#define MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_FILESYSTEM                    default                                          // set by library:storage_filesystem_no_rbp
#define MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_FOLDER_PATH                   kvstore                                          // set by library:storage_filesystem_no_rbp
#define MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_MOUNT_POINT                   kv                                               // set by library:storage_filesystem_no_rbp
#define MBED_CONF_STORAGE_FILESYSTEM_RBP_INTERNAL_SIZE                    0                                                // set by library:storage_filesystem
#define MBED_CONF_STORAGE_STORAGE_TYPE                                    default                                          // set by library:storage
#define MBED_CONF_STORAGE_TDB_EXTERNAL_BLOCKDEVICE                        default                                          // set by library:storage_tdb_external
#define MBED_CONF_STORAGE_TDB_EXTERNAL_EXTERNAL_BASE_ADDRESS              0                                                // set by library:storage_tdb_external
#define MBED_CONF_STORAGE_TDB_EXTERNAL_EXTERNAL_SIZE                      0                                                // set by library:storage_tdb_external
#define MBED_CONF_STORAGE_TDB_EXTERNAL_INTERNAL_BASE_ADDRESS              0                                                // set by library:storage_tdb_external
#define MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_BLOCKDEVICE                 default                                          // set by library:storage_tdb_external_no_rbp
#define MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_EXTERNAL_BASE_ADDRESS       0                                                // set by library:storage_tdb_external_no_rbp
#define MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_EXTERNAL_SIZE               0                                                // set by library:storage_tdb_external_no_rbp
#define MBED_CONF_STORAGE_TDB_EXTERNAL_RBP_INTERNAL_SIZE                  0                                                // set by library:storage_tdb_external
#define MBED_CONF_STORAGE_TDB_INTERNAL_INTERNAL_BASE_ADDRESS              0                                                // set by library:storage_tdb_internal
#define MBED_CONF_STORAGE_TDB_INTERNAL_INTERNAL_SIZE                      0                                                // set by library:storage_tdb_internal
#define MBED_CONF_TARGET_BOOT_STACK_SIZE                                  0x400                                            // set by library:rtos[*]
#define MBED_CONF_TARGET_CONSOLE_UART                                     1                                                // set by target:Target
#define MBED_CONF_TARGET_DEEP_SLEEP_LATENCY                               4                                                // set by target:MCU_STM32
#define MBED_CONF_TARGET_DEFAULT_ADC_VREF                                 NAN                                              // set by target:Target
#define MBED_CONF_TARGET_FLASH_DUAL_BANK                                  0                                                // set by target:DISCO_F769NI
#define MBED_CONF_TARGET_INIT_US_TICKER_AT_BOOT                           1                                                // set by target:MCU_STM32
#define MBED_CONF_TARGET_LPTICKER_LPTIM                                   1                                                // set by target:DISCO_F769NI
#define MBED_CONF_TARGET_LPTICKER_LPTIM_CLOCK                             1                                                // set by target:MCU_STM32
#define MBED_CONF_TARGET_LPUART_CLOCK_SOURCE                              USE_LPUART_CLK_LSE|USE_LPUART_CLK_PCLK1          // set by target:MCU_STM32
#define MBED_CONF_TARGET_LSE_AVAILABLE                                    1                                                // set by target:MCU_STM32
#define MBED_CONF_TARGET_MPU_ROM_END                                      0x0fffffff                                       // set by target:Target
#define MBED_CONF_TARGET_NETWORK_DEFAULT_INTERFACE_TYPE                   ETHERNET                                         // set by target:DISCO_F769NI
#define MBED_CONF_TARGET_TICKLESS_FROM_US_TICKER                          0                                                // set by target:Target
#define MBED_CONF_TARGET_XIP_ENABLE                                       0                                                // set by target:Target
#define MBED_CRC_TABLE_SIZE                                               16                                               // set by library:drivers
#define MBED_LFS2_BLOCK_CYCLES                                            1024                                             // set by library:littlefs2
#define MBED_LFS2_BLOCK_SIZE                                              512                                              // set by library:littlefs2
#define MBED_LFS2_CACHE_SIZE                                              64                                               // set by library:littlefs2
#define MBED_LFS2_ENABLE_INFO                                             0                                                // set by library:littlefs2
#define MBED_LFS2_INTRINSICS                                              1                                                // set by library:littlefs2
#define MBED_LFS2_LOOKAHEAD_SIZE                                          64                                               // set by library:littlefs2
#define MBED_LFS_BLOCK_SIZE                                               512                                              // set by library:littlefs
#define MBED_LFS_ENABLE_INFO                                              0                                                // set by library:littlefs
#define MBED_LFS_INTRINSICS                                               1                                                // set by library:littlefs
#define MBED_LFS_LOOKAHEAD                                                512                                              // set by library:littlefs
#define MBED_LFS_PROG_SIZE                                                64                                               // set by library:littlefs
#define MBED_LFS_READ_SIZE                                                64                                               // set by library:littlefs
#define MBED_STACK_DUMP_ENABLED                                           0                                                // set by library:platform
#define MEM_ALLOC                                                         malloc                                           // set by library:mbed-trace
#define MEM_FREE                                                          free                                             // set by library:mbed-trace
// Macros
#define MBEDTLS_CIPHER_MODE_CTR                                                                                            // defined by library:SecureStore
#define MBEDTLS_CMAC_C                                                                                                     // defined by library:SecureStore
#define UNITY_INCLUDE_CONFIG_H                                                                                             // defined by library:utest
#define _RTE_                                                                                                              // defined by library:rtos

#endif
