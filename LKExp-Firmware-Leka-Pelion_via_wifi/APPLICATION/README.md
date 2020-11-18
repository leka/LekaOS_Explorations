# LKExp - Update LEKA via Pelion

## How to run

Deploy all necessary libs, then add API key (not given here because it is a public repo).

LEKA is a custom target, you have to define it in custom_targets.json and associated files. Here it will have the name of `LEKA_V1_0_DEV`.

Do not forget to manually add in mbed_app.json SSID and Password of Wifi.

Before flashing, make sure to enable device-management feature with `mbed dm init -d "leka.io" --model-name "LEKA_WIFI" --force -q` 

Compile and flash with openocd if it is the first time, compile and update otherwise.

To update, you can apply the command `mbed dm update device -D <Device_ID> --custom-targets ./` in APPLICATION folder, the Device ID is given by the flashed board at booting when connected to Internet and is also available on Portal of Pelion.
