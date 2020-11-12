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
    "mbed-bootloader.bootloader-size"  : "(64*1024)",
    "target.restrict_size"             : "0x10000",
    "update-client.application-details": "(MBED_ROM_START + 256*1024)"
}
```

then compile with `mbed compile --app-config=configs/external_kvstore_with_qspif.json` after completed target and toolchain
