# LKExp - Update DISCO_F769NI via Pelion

## How to run (APPLICATION)

Deploy all necessary libs, then add API key (not given here because it is a public repo).

Before flashing, make sure to enable device-management feature with `mbed dm init -d "leka.io" --model-name "DISCO_ETHERNET" --force -q` 

Compile and flash if it is the first time, compile and update otherwise.

To update, you can apply the command `mbed dm update device -D <Device_ID>` where the Device ID is given by the flashed board at booting when connected to Internet.

To check if the update actually happend, you can change state of LED2 in main.cpp in checkAlive function.

## How to run (BOOTLOADER)

Clone mbed-bootloader [[Link](https://github.com/ARMmbed/mbed-bootloader)] and deploy dependencies

Add in configs/external_kvstore_with_qspif.json:

```cpp
"DISCO_F769NI": {
    "target.extra_labels_remove"       : [ "PSA" ],
    "mbed-bootloader.bootloader-size"  : "MBED_BOOTLOADER_SIZE",
    "update-client.application-details": "(MBED_ROM_START + MBED_BOOTLOADER_SIZE)",
    "mbed-bootloader.max-application-size"      : "MBED_ROM_SIZE - MBED_BOOTLOADER_SIZE - APP_KVSTORE_SIZE - MBED_BOOTLOADER_ACTIVE_HEADER_REGION_SIZE",
    "mbed-bootloader.use-kvstore-rot"           : 1,
    "mbed-bootloader.application-start-address" : "(MBED_ROM_START + MBED_BOOTLOADER_SIZE + MBED_BOOTLOADER_ACTIVE_HEADER_REGION_SIZE)",
    "target.restrict_size"                      : "0x10000",
    "update-client.storage-locations"           : 1,
    "update-client.storage-address"             : "(0x0 + MBED_CONF_STORAGE_FILESYSTEM_EXTERNAL_SIZE)",
    "update-client.storage-size"                : "((MBED_ROM_START + MBED_ROM_SIZE - MBED_CONF_MBED_BOOTLOADER_APPLICATION_START_ADDRESS - APP_KVSTORE_SIZE) * MBED_CONF_UPDATE_CLIENT_STORAGE_LOCATIONS)",

    "storage.storage_type"                      : "FILESYSTEM",
    "storage_filesystem.internal_base_address"  : "(MBED_ROM_START + 0x180000)",
    "storage_filesystem.rbp_internal_size"      : "(APP_KVSTORE_SIZE)",

    "storage_filesystem.filesystem"             : "LITTLE",
    "storage_filesystem.blockdevice"            : "QSPIF",
    "storage_filesystem.external_size"          : "(1024 * 1024)",
    "storage_filesystem.external_base_address"  : "(0)",
    "target.components_add"                     : ["QSPIF"],

    "update-client.firmware-header-version"     : "2"
        }
```

with MACRO `"MBED_BOOTLOADER_SIZE=(2* 32*1024)"`

then compile with `mbed compile --app-config=configs/external_kvstore_with_qspif.json` after completed target and toolchain
