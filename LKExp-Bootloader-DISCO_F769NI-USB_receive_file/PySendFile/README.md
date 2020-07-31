# LKExp-mbed-simple_bootloader

## Goal

Create an application starter, that will launch the main application

## Ressources

GitHub: [Mbed OS bootloader example](https://github.com/ARMmbed/mbed-os-example-bootloader)

Mbed website: [Creating and using a bootloader](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/creating-and-using-a-bootloader.html)

Mbed website: [Bootloader configuration](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/bootloader-configuration.html)

Mbed forum: [Bootloader tutorial not working with certain targets](https://forums.mbed.com/t/bootloader-tutorial-not-working-with-certain-targets/9104)

## Preparation

First you need to define the maximum bootloader size by setting the value of `target.restrict_size` in the `target_overrides` section in `mbed_app.json`:
```
"target_overrides": {
	"*": {
		"target.restrict_size": "0x10000",
		...
	}
```
This allows you to use the `POST_APPLICATION_ADDR` parameter corresponding to the bootloader ending address, which you can use to launch your main application by calling `mbed_start_application(POST_APPLICATION_ADDR);` at the end of the bootloader main function.

Build the bootloader application with the command `mbed compile` and find `<project_name>.bin` in `./BUILD/DISCO_F769NI/GCC_ARM/`, that is the binary image of your bootloader.

## Usage

In a project you want to use the bootloader with, create a folder named `bootloader` and place the bootloader binary image in. Rename it `bootloader.bin` for simplicity.

In the file `mbed_app.json` of this project, add the path of the bootloader image by setting `target.bootloader_img` in the `target_overrides` section:
```
"target_overrides": {
	"*": {
		"target.bootloader_img": "bootloader/bootloader.bin",
		...
	}
```

Make sure the DISCO_F769NI board is connected to your PC over USB and detected. Use the command `mbed compile -f` to build the application with the bootloader and flash it to the board. You can then see your main application run after the bootloader.

## Parameters and functions

### 1. In the bootloader project

#### 1.1. In `mbed_app.json`

The following parameter should be written in the `"target_overrides"` section.

`"target.restrict_size"`: sets the maximum size of the bootloader image. It defines the symbols `APPLICATION_ADDR`, `APPLICATION_SIZE`, `POST_APPLICATION_ADDR` and `POST_APPLICATION_SIZE`, making the ROM look like this:
```
|-------------------|   POST_APPLICATION_ADDR + POST_APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|                   |
|     Reserved      |
|                   |
+-------------------+   POST_APPLICATION_ADDR == APPLICATION_ADDR + APPLICATION_SIZE
|                   |
|    Application    |
|  (the bootloader) |
|                   |
+-------------------+   APPLICATION_ADDR == Start of ROM
```
The value we write for this parameter is directly given to `APPLICATION_SIZE`.

#### 1.2. In `src/main.cpp`

`mbed_start_application(POST_APPLICATION_ADDR)`: starts the application at the address `POST_APPLICATION_ADDR`, corresponding to the end of the bootloader memory.

### 2. In the main application project

#### In `mbed_app.json`

The following parameters should be written in the `"target_overrides"` section.

-	`"target.bootloader_img"`: path to the bootloader binary image. It redefines the previous symbols as `BOOTLOADER_ADDR`, `BOOTLOADER_SIZE`, `APPLICATION_ADDR` and `APPLICATION_SIZE`. The ROM now looks like this:
```
|-------------------|   APPLICATION_ADDR + APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|    Application    |
|   (main program)  |
|                   |
+-------------------+   APPLICATION_ADDR == BOOTLOADER_ADDR + BOOTLOADER_SIZE
|                   |
|    Bootloader     |
|    (<path to      |
|  bootloader bin>) |
|                   |
+-------------------+   BOOTLOADER_ADDR == Start of ROM
```

-	`"target.mbed_app_start"` or `"target.app_offset"`: sets the start address of the main application, creating space between the bootloader and the application. Be careful to set a value that does not get in conflict with the memory taken by the bootloader (`"target.restrict_size"`). Note that `"target.app_offset"` is relative to the start of ROM, which is not at the address `0x0`.

-	`"target.mbed_app_size"`: sets the maximum size of the application. In conjunction with the previous parameter, it also sets the end address of the application.

## Tests results

### Basic test

Test conditions:

-	`"target.restrict_size": "0x10000"` in `mbed_app.json` in bootloader project
-	Main application: prints "Hello World" every second

Results:

-	Application runs right after the bootloader
-	Bootloader starts at address `0x8000000` and application at address `0x8010000`

### Modifying the parameters

Tests conditions: main application prints "Hello World" every second

#### 1. `"target.restrict_size"`

Results:

-	With `"target.restrict_size"` between `0x8000` and `0xFFFF`
	-	`APPLICATION_SIZE` is `0x8000`, `POST_APPLICATION_ADDR` is `0x8008000`
	-	Compile fails, bootloader does not fit in allocated ROM (needs `0x8A64` bytes)
	
-	With `"target.restrict_size"` between `0x10000` and `0x1FFFF`
	-	`APPLICATION_SIZE` is `0x10000`, `POST_APPLICATION_ADDR` is `0x8010000`
	-	Compiler succeeds, application runs right after the bootloader
	
-	With `"target.restrict_size"` between `0x20000` and `0x3FFFF`
	-	`APPLICATION_SIZE` is `0x20000`, `POST_APPLICATION_ADDR` is `0x8020000`
	-	Compiler succeeds, bootloader runs but not the application (also with `"target.restrict_size"` above `0x40000`)

#### 2. `"target.restrict_size"` and `"target.app_offset"`

Results:

-	With `"target.restrict_size": "0x10000"` and `"target.app_offset": "0x10000"` (or whatever value with **`"target.restrict_size"`==`"target.app_offset"`**)
	-	`BOOTLOADER_SIZE` is the value given to `"target.restrict_size"` and `"target.app_offset"`
	-	Compiler succeeds, application runs right after the bootloader

-	With `"target.restrict_size"` < `"target.app_offset"`
	-	`BOOTLOADER_SIZE` is `0x10000`, `APPLICATION_ADDR` is the value given to `"target.app_offset"`
	-	Compiler succeeds, bootloader runs but not the application

-	With `"target.restrict_size"` > `"target.app_offset"`
	-	Compiler fails, application cannot be placed before the end of bootloader.


# LKExp-mbed-simple_bootloader

## Goal

Create an application starter, that will launch the main application

## Ressources

GitHub: [Mbed OS bootloader example](https://github.com/ARMmbed/mbed-os-example-bootloader)

Mbed website: [Creating and using a bootloader](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/creating-and-using-a-bootloader.html)

Mbed website: [Bootloader configuration](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/bootloader-configuration.html)

Mbed forum: [Bootloader tutorial not working with certain targets](https://forums.mbed.com/t/bootloader-tutorial-not-working-with-certain-targets/9104)

## Preparation

First you need to define the maximum bootloader size by setting the value of `target.restrict_size` in the `target_overrides` section in `mbed_app.json`:
```
"target_overrides": {
	"*": {
		"target.restrict_size": "0x10000",
		...
	}
```
This allows you to use the `POST_APPLICATION_ADDR` parameter corresponding to the bootloader ending address, which you can use to launch your main application by calling `mbed_start_application(POST_APPLICATION_ADDR);` at the end of the bootloader main function.

Build the bootloader application with the command `mbed compile` and find `<project_name>.bin` in `./BUILD/DISCO_F769NI/GCC_ARM/`, that is the binary image of your bootloader.

## Usage

In a project you want to use the bootloader with, create a folder named `bootloader` and place the bootloader binary image in. Rename it `bootloader.bin` for simplicity.

In the file `mbed_app.json` of this project, add the path of the bootloader image by setting `target.bootloader_img` in the `target_overrides` section:
```
"target_overrides": {
	"*": {
		"target.bootloader_img": "bootloader/bootloader.bin",
		...
	}
```

Make sure the DISCO_F769NI board is connected to your PC over USB and detected. Use the command `mbed compile -f` to build the application with the bootloader and flash it to the board. You can then see your main application run after the bootloader.

## Parameters and functions

### 1. In the bootloader project

#### 1.1. In `mbed_app.json`

The following parameter should be written in the `"target_overrides"` section.

`"target.restrict_size"`: sets the maximum size of the bootloader image. It defines the symbols `APPLICATION_ADDR`, `APPLICATION_SIZE`, `POST_APPLICATION_ADDR` and `POST_APPLICATION_SIZE`, making the ROM look like this:
```
|-------------------|   POST_APPLICATION_ADDR + POST_APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|                   |
|     Reserved      |
|                   |
+-------------------+   POST_APPLICATION_ADDR == APPLICATION_ADDR + APPLICATION_SIZE
|                   |
|    Application    |
|  (the bootloader) |
|                   |
+-------------------+   APPLICATION_ADDR == Start of ROM
```
The value we write for this parameter is directly given to `APPLICATION_SIZE`.

#### 1.2. In `src/main.cpp`

`mbed_start_application(POST_APPLICATION_ADDR)`: starts the application at the address `POST_APPLICATION_ADDR`, corresponding to the end of the bootloader memory.

### 2. In the main application project

#### In `mbed_app.json`

The following parameters should be written in the `"target_overrides"` section.

-	`"target.bootloader_img"`: path to the bootloader binary image. It redefines the previous symbols as `BOOTLOADER_ADDR`, `BOOTLOADER_SIZE`, `APPLICATION_ADDR` and `APPLICATION_SIZE`. The ROM now looks like this:
```
|-------------------|   APPLICATION_ADDR + APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|    Application    |
|   (main program)  |
|                   |
+-------------------+   APPLICATION_ADDR == BOOTLOADER_ADDR + BOOTLOADER_SIZE
|                   |
|    Bootloader     |
|    (<path to      |
|  bootloader bin>) |
|                   |
+-------------------+   BOOTLOADER_ADDR == Start of ROM
```

-	`"target.mbed_app_start"` or `"target.app_offset"`: sets the start address of the main application, creating space between the bootloader and the application. Be careful to set a value that does not get in conflict with the memory taken by the bootloader (`"target.restrict_size"`). Note that `"target.app_offset"` is relative to the start of ROM, which is not at the address `0x0`.

-	`"target.mbed_app_size"`: sets the maximum size of the application. In conjunction with the previous parameter, it also sets the end address of the application.

## Tests results

### Basic test

Test conditions:

-	`"target.restrict_size": "0x10000"` in `mbed_app.json` in bootloader project
-	Main application: prints "Hello World" every second

Results:

-	Application runs right after the bootloader
-	Bootloader starts at address `0x8000000` and application at address `0x8010000`

### Modifying the parameters

Tests conditions: main application prints "Hello World" every second

#### 1. `"target.restrict_size"`

Results:

-	With `"target.restrict_size"` between `0x8000` and `0xFFFF`
	-	`APPLICATION_SIZE` is `0x8000`, `POST_APPLICATION_ADDR` is `0x8008000`
	-	Compile fails, bootloader does not fit in allocated ROM (needs `0x8A64` bytes)
	
-	With `"target.restrict_size"` between `0x10000` and `0x1FFFF`
	-	`APPLICATION_SIZE` is `0x10000`, `POST_APPLICATION_ADDR` is `0x8010000`
	-	Compiler succeeds, application runs right after the bootloader
	
-	With `"target.restrict_size"` between `0x20000` and `0x3FFFF`
	-	`APPLICATION_SIZE` is `0x20000`, `POST_APPLICATION_ADDR` is `0x8020000`
	-	Compiler succeeds, bootloader runs but not the application (also with `"target.restrict_size"` above `0x40000`)

#### 2. `"target.restrict_size"` and `"target.app_offset"`

Results:

-	With `"target.restrict_size": "0x10000"` and `"target.app_offset": "0x10000"` (or whatever value with **`"target.restrict_size"`==`"target.app_offset"`**)
	-	`BOOTLOADER_SIZE` is the value given to `"target.restrict_size"` and `"target.app_offset"`
	-	Compiler succeeds, application runs right after the bootloader

-	With `"target.restrict_size"` < `"target.app_offset"`
	-	`BOOTLOADER_SIZE` is `0x10000`, `APPLICATION_ADDR` is the value given to `"target.app_offset"`
	-	Compiler succeeds, bootloader runs but not the application

-	With `"target.restrict_size"` > `"target.app_offset"`
	-	Compiler fails, application cannot be placed before the end of bootloader.



# LKExp-mbed-simple_bootloader

## Goal

Create an application starter, that will launch the main application

## Ressources

GitHub: [Mbed OS bootloader example](https://github.com/ARMmbed/mbed-os-example-bootloader)

Mbed website: [Creating and using a bootloader](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/creating-and-using-a-bootloader.html)

Mbed website: [Bootloader configuration](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/bootloader-configuration.html)

Mbed forum: [Bootloader tutorial not working with certain targets](https://forums.mbed.com/t/bootloader-tutorial-not-working-with-certain-targets/9104)

## Preparation

First you need to define the maximum bootloader size by setting the value of `target.restrict_size` in the `target_overrides` section in `mbed_app.json`:
```
"target_overrides": {
	"*": {
		"target.restrict_size": "0x10000",
		...
	}
```
This allows you to use the `POST_APPLICATION_ADDR` parameter corresponding to the bootloader ending address, which you can use to launch your main application by calling `mbed_start_application(POST_APPLICATION_ADDR);` at the end of the bootloader main function.

Build the bootloader application with the command `mbed compile` and find `<project_name>.bin` in `./BUILD/DISCO_F769NI/GCC_ARM/`, that is the binary image of your bootloader.

## Usage

In a project you want to use the bootloader with, create a folder named `bootloader` and place the bootloader binary image in. Rename it `bootloader.bin` for simplicity.

In the file `mbed_app.json` of this project, add the path of the bootloader image by setting `target.bootloader_img` in the `target_overrides` section:
```
"target_overrides": {
	"*": {
		"target.bootloader_img": "bootloader/bootloader.bin",
		...
	}
```

Make sure the DISCO_F769NI board is connected to your PC over USB and detected. Use the command `mbed compile -f` to build the application with the bootloader and flash it to the board. You can then see your main application run after the bootloader.

## Parameters and functions

### 1. In the bootloader project

#### 1.1. In `mbed_app.json`

The following parameter should be written in the `"target_overrides"` section.

`"target.restrict_size"`: sets the maximum size of the bootloader image. It defines the symbols `APPLICATION_ADDR`, `APPLICATION_SIZE`, `POST_APPLICATION_ADDR` and `POST_APPLICATION_SIZE`, making the ROM look like this:
```
|-------------------|   POST_APPLICATION_ADDR + POST_APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|                   |
|     Reserved      |
|                   |
+-------------------+   POST_APPLICATION_ADDR == APPLICATION_ADDR + APPLICATION_SIZE
|                   |
|    Application    |
|  (the bootloader) |
|                   |
+-------------------+   APPLICATION_ADDR == Start of ROM
```
The value we write for this parameter is directly given to `APPLICATION_SIZE`.

#### 1.2. In `src/main.cpp`

`mbed_start_application(POST_APPLICATION_ADDR)`: starts the application at the address `POST_APPLICATION_ADDR`, corresponding to the end of the bootloader memory.

### 2. In the main application project

#### In `mbed_app.json`

The following parameters should be written in the `"target_overrides"` section.

-	`"target.bootloader_img"`: path to the bootloader binary image. It redefines the previous symbols as `BOOTLOADER_ADDR`, `BOOTLOADER_SIZE`, `APPLICATION_ADDR` and `APPLICATION_SIZE`. The ROM now looks like this:
```
|-------------------|   APPLICATION_ADDR + APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|    Application    |
|   (main program)  |
|                   |
+-------------------+   APPLICATION_ADDR == BOOTLOADER_ADDR + BOOTLOADER_SIZE
|                   |
|    Bootloader     |
|    (<path to      |
|  bootloader bin>) |
|                   |
+-------------------+   BOOTLOADER_ADDR == Start of ROM
```

-	`"target.mbed_app_start"` or `"target.app_offset"`: sets the start address of the main application, creating space between the bootloader and the application. Be careful to set a value that does not get in conflict with the memory taken by the bootloader (`"target.restrict_size"`). Note that `"target.app_offset"` is relative to the start of ROM, which is not at the address `0x0`.

-	`"target.mbed_app_size"`: sets the maximum size of the application. In conjunction with the previous parameter, it also sets the end address of the application.

## Tests results

### Basic test

Test conditions:

-	`"target.restrict_size": "0x10000"` in `mbed_app.json` in bootloader project
-	Main application: prints "Hello World" every second

Results:

-	Application runs right after the bootloader
-	Bootloader starts at address `0x8000000` and application at address `0x8010000`

### Modifying the parameters

Tests conditions: main application prints "Hello World" every second

#### 1. `"target.restrict_size"`

Results:

-	With `"target.restrict_size"` between `0x8000` and `0xFFFF`
	-	`APPLICATION_SIZE` is `0x8000`, `POST_APPLICATION_ADDR` is `0x8008000`
	-	Compile fails, bootloader does not fit in allocated ROM (needs `0x8A64` bytes)
	
-	With `"target.restrict_size"` between `0x10000` and `0x1FFFF`
	-	`APPLICATION_SIZE` is `0x10000`, `POST_APPLICATION_ADDR` is `0x8010000`
	-	Compiler succeeds, application runs right after the bootloader
	
-	With `"target.restrict_size"` between `0x20000` and `0x3FFFF`
	-	`APPLICATION_SIZE` is `0x20000`, `POST_APPLICATION_ADDR` is `0x8020000`
	-	Compiler succeeds, bootloader runs but not the application (also with `"target.restrict_size"` above `0x40000`)

#### 2. `"target.restrict_size"` and `"target.app_offset"`

Results:

-	With `"target.restrict_size": "0x10000"` and `"target.app_offset": "0x10000"` (or whatever value with **`"target.restrict_size"`==`"target.app_offset"`**)
	-	`BOOTLOADER_SIZE` is the value given to `"target.restrict_size"` and `"target.app_offset"`
	-	Compiler succeeds, application runs right after the bootloader

-	With `"target.restrict_size"` < `"target.app_offset"`
	-	`BOOTLOADER_SIZE` is `0x10000`, `APPLICATION_ADDR` is the value given to `"target.app_offset"`
	-	Compiler succeeds, bootloader runs but not the application

-	With `"target.restrict_size"` > `"target.app_offset"`
	-	Compiler fails, application cannot be placed before the end of bootloader.


# LKExp-mbed-simple_bootloader

## Goal

Create an application starter, that will launch the main application

## Ressources

GitHub: [Mbed OS bootloader example](https://github.com/ARMmbed/mbed-os-example-bootloader)

Mbed website: [Creating and using a bootloader](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/creating-and-using-a-bootloader.html)

Mbed website: [Bootloader configuration](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/bootloader-configuration.html)

Mbed forum: [Bootloader tutorial not working with certain targets](https://forums.mbed.com/t/bootloader-tutorial-not-working-with-certain-targets/9104)

## Preparation

First you need to define the maximum bootloader size by setting the value of `target.restrict_size` in the `target_overrides` section in `mbed_app.json`:
```
"target_overrides": {
	"*": {
		"target.restrict_size": "0x10000",
		...
	}
```
This allows you to use the `POST_APPLICATION_ADDR` parameter corresponding to the bootloader ending address, which you can use to launch your main application by calling `mbed_start_application(POST_APPLICATION_ADDR);` at the end of the bootloader main function.

Build the bootloader application with the command `mbed compile` and find `<project_name>.bin` in `./BUILD/DISCO_F769NI/GCC_ARM/`, that is the binary image of your bootloader.

## Usage

In a project you want to use the bootloader with, create a folder named `bootloader` and place the bootloader binary image in. Rename it `bootloader.bin` for simplicity.

In the file `mbed_app.json` of this project, add the path of the bootloader image by setting `target.bootloader_img` in the `target_overrides` section:
```
"target_overrides": {
	"*": {
		"target.bootloader_img": "bootloader/bootloader.bin",
		...
	}
```

Make sure the DISCO_F769NI board is connected to your PC over USB and detected. Use the command `mbed compile -f` to build the application with the bootloader and flash it to the board. You can then see your main application run after the bootloader.

## Parameters and functions

### 1. In the bootloader project

#### 1.1. In `mbed_app.json`

The following parameter should be written in the `"target_overrides"` section.

`"target.restrict_size"`: sets the maximum size of the bootloader image. It defines the symbols `APPLICATION_ADDR`, `APPLICATION_SIZE`, `POST_APPLICATION_ADDR` and `POST_APPLICATION_SIZE`, making the ROM look like this:
```
|-------------------|   POST_APPLICATION_ADDR + POST_APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|                   |
|     Reserved      |
|                   |
+-------------------+   POST_APPLICATION_ADDR == APPLICATION_ADDR + APPLICATION_SIZE
|                   |
|    Application    |
|  (the bootloader) |
|                   |
+-------------------+   APPLICATION_ADDR == Start of ROM
```
The value we write for this parameter is directly given to `APPLICATION_SIZE`.

#### 1.2. In `src/main.cpp`

`mbed_start_application(POST_APPLICATION_ADDR)`: starts the application at the address `POST_APPLICATION_ADDR`, corresponding to the end of the bootloader memory.

### 2. In the main application project

#### In `mbed_app.json`

The following parameters should be written in the `"target_overrides"` section.

-	`"target.bootloader_img"`: path to the bootloader binary image. It redefines the previous symbols as `BOOTLOADER_ADDR`, `BOOTLOADER_SIZE`, `APPLICATION_ADDR` and `APPLICATION_SIZE`. The ROM now looks like this:
```
|-------------------|   APPLICATION_ADDR + APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|    Application    |
|   (main program)  |
|                   |
+-------------------+   APPLICATION_ADDR == BOOTLOADER_ADDR + BOOTLOADER_SIZE
|                   |
|    Bootloader     |
|    (<path to      |
|  bootloader bin>) |
|                   |
+-------------------+   BOOTLOADER_ADDR == Start of ROM
```

-	`"target.mbed_app_start"` or `"target.app_offset"`: sets the start address of the main application, creating space between the bootloader and the application. Be careful to set a value that does not get in conflict with the memory taken by the bootloader (`"target.restrict_size"`). Note that `"target.app_offset"` is relative to the start of ROM, which is not at the address `0x0`.

-	`"target.mbed_app_size"`: sets the maximum size of the application. In conjunction with the previous parameter, it also sets the end address of the application.

## Tests results

### Basic test

Test conditions:

-	`"target.restrict_size": "0x10000"` in `mbed_app.json` in bootloader project
-	Main application: prints "Hello World" every second

Results:

-	Application runs right after the bootloader
-	Bootloader starts at address `0x8000000` and application at address `0x8010000`

### Modifying the parameters

Tests conditions: main application prints "Hello World" every second

#### 1. `"target.restrict_size"`

Results:

-	With `"target.restrict_size"` between `0x8000` and `0xFFFF`
	-	`APPLICATION_SIZE` is `0x8000`, `POST_APPLICATION_ADDR` is `0x8008000`
	-	Compile fails, bootloader does not fit in allocated ROM (needs `0x8A64` bytes)
	
-	With `"target.restrict_size"` between `0x10000` and `0x1FFFF`
	-	`APPLICATION_SIZE` is `0x10000`, `POST_APPLICATION_ADDR` is `0x8010000`
	-	Compiler succeeds, application runs right after the bootloader
	
-	With `"target.restrict_size"` between `0x20000` and `0x3FFFF`
	-	`APPLICATION_SIZE` is `0x20000`, `POST_APPLICATION_ADDR` is `0x8020000`
	-	Compiler succeeds, bootloader runs but not the application (also with `"target.restrict_size"` above `0x40000`)

#### 2. `"target.restrict_size"` and `"target.app_offset"`

Results:

-	With `"target.restrict_size": "0x10000"` and `"target.app_offset": "0x10000"` (or whatever value with **`"target.restrict_size"`==`"target.app_offset"`**)
	-	`BOOTLOADER_SIZE` is the value given to `"target.restrict_size"` and `"target.app_offset"`
	-	Compiler succeeds, application runs right after the bootloader

-	With `"target.restrict_size"` < `"target.app_offset"`
	-	`BOOTLOADER_SIZE` is `0x10000`, `APPLICATION_ADDR` is the value given to `"target.app_offset"`
	-	Compiler succeeds, bootloader runs but not the application

-	With `"target.restrict_size"` > `"target.app_offset"`
	-	Compiler fails, application cannot be placed before the end of bootloader.




# LKExp-mbed-simple_bootloader

## Goal

Create an application starter, that will launch the main application

## Ressources

GitHub: [Mbed OS bootloader example](https://github.com/ARMmbed/mbed-os-example-bootloader)

Mbed website: [Creating and using a bootloader](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/creating-and-using-a-bootloader.html)

Mbed website: [Bootloader configuration](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/bootloader-configuration.html)

Mbed forum: [Bootloader tutorial not working with certain targets](https://forums.mbed.com/t/bootloader-tutorial-not-working-with-certain-targets/9104)

## Preparation

First you need to define the maximum bootloader size by setting the value of `target.restrict_size` in the `target_overrides` section in `mbed_app.json`:
```
"target_overrides": {
	"*": {
		"target.restrict_size": "0x10000",
		...
	}
```
This allows you to use the `POST_APPLICATION_ADDR` parameter corresponding to the bootloader ending address, which you can use to launch your main application by calling `mbed_start_application(POST_APPLICATION_ADDR);` at the end of the bootloader main function.

Build the bootloader application with the command `mbed compile` and find `<project_name>.bin` in `./BUILD/DISCO_F769NI/GCC_ARM/`, that is the binary image of your bootloader.

## Usage

In a project you want to use the bootloader with, create a folder named `bootloader` and place the bootloader binary image in. Rename it `bootloader.bin` for simplicity.

In the file `mbed_app.json` of this project, add the path of the bootloader image by setting `target.bootloader_img` in the `target_overrides` section:
```
"target_overrides": {
	"*": {
		"target.bootloader_img": "bootloader/bootloader.bin",
		...
	}
```

Make sure the DISCO_F769NI board is connected to your PC over USB and detected. Use the command `mbed compile -f` to build the application with the bootloader and flash it to the board. You can then see your main application run after the bootloader.

## Parameters and functions

### 1. In the bootloader project

#### 1.1. In `mbed_app.json`

The following parameter should be written in the `"target_overrides"` section.

`"target.restrict_size"`: sets the maximum size of the bootloader image. It defines the symbols `APPLICATION_ADDR`, `APPLICATION_SIZE`, `POST_APPLICATION_ADDR` and `POST_APPLICATION_SIZE`, making the ROM look like this:
```
|-------------------|   POST_APPLICATION_ADDR + POST_APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|                   |
|     Reserved      |
|                   |
+-------------------+   POST_APPLICATION_ADDR == APPLICATION_ADDR + APPLICATION_SIZE
|                   |
|    Application    |
|  (the bootloader) |
|                   |
+-------------------+   APPLICATION_ADDR == Start of ROM
```
The value we write for this parameter is directly given to `APPLICATION_SIZE`.

#### 1.2. In `src/main.cpp`

`mbed_start_application(POST_APPLICATION_ADDR)`: starts the application at the address `POST_APPLICATION_ADDR`, corresponding to the end of the bootloader memory.

### 2. In the main application project

#### In `mbed_app.json`

The following parameters should be written in the `"target_overrides"` section.

-	`"target.bootloader_img"`: path to the bootloader binary image. It redefines the previous symbols as `BOOTLOADER_ADDR`, `BOOTLOADER_SIZE`, `APPLICATION_ADDR` and `APPLICATION_SIZE`. The ROM now looks like this:
```
|-------------------|   APPLICATION_ADDR + APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|    Application    |
|   (main program)  |
|                   |
+-------------------+   APPLICATION_ADDR == BOOTLOADER_ADDR + BOOTLOADER_SIZE
|                   |
|    Bootloader     |
|    (<path to      |
|  bootloader bin>) |
|                   |
+-------------------+   BOOTLOADER_ADDR == Start of ROM
```

-	`"target.mbed_app_start"` or `"target.app_offset"`: sets the start address of the main application, creating space between the bootloader and the application. Be careful to set a value that does not get in conflict with the memory taken by the bootloader (`"target.restrict_size"`). Note that `"target.app_offset"` is relative to the start of ROM, which is not at the address `0x0`.

-	`"target.mbed_app_size"`: sets the maximum size of the application. In conjunction with the previous parameter, it also sets the end address of the application.

## Tests results

### Basic test

Test conditions:

-	`"target.restrict_size": "0x10000"` in `mbed_app.json` in bootloader project
-	Main application: prints "Hello World" every second

Results:

-	Application runs right after the bootloader
-	Bootloader starts at address `0x8000000` and application at address `0x8010000`

### Modifying the parameters

Tests conditions: main application prints "Hello World" every second

#### 1. `"target.restrict_size"`

Results:

-	With `"target.restrict_size"` between `0x8000` and `0xFFFF`
	-	`APPLICATION_SIZE` is `0x8000`, `POST_APPLICATION_ADDR` is `0x8008000`
	-	Compile fails, bootloader does not fit in allocated ROM (needs `0x8A64` bytes)
	
-	With `"target.restrict_size"` between `0x10000` and `0x1FFFF`
	-	`APPLICATION_SIZE` is `0x10000`, `POST_APPLICATION_ADDR` is `0x8010000`
	-	Compiler succeeds, application runs right after the bootloader
	
-	With `"target.restrict_size"` between `0x20000` and `0x3FFFF`
	-	`APPLICATION_SIZE` is `0x20000`, `POST_APPLICATION_ADDR` is `0x8020000`
	-	Compiler succeeds, bootloader runs but not the application (also with `"target.restrict_size"` above `0x40000`)

#### 2. `"target.restrict_size"` and `"target.app_offset"`

Results:

-	With `"target.restrict_size": "0x10000"` and `"target.app_offset": "0x10000"` (or whatever value with **`"target.restrict_size"`==`"target.app_offset"`**)
	-	`BOOTLOADER_SIZE` is the value given to `"target.restrict_size"` and `"target.app_offset"`
	-	Compiler succeeds, application runs right after the bootloader

-	With `"target.restrict_size"` < `"target.app_offset"`
	-	`BOOTLOADER_SIZE` is `0x10000`, `APPLICATION_ADDR` is the value given to `"target.app_offset"`
	-	Compiler succeeds, bootloader runs but not the application

-	With `"target.restrict_size"` > `"target.app_offset"`
	-	Compiler fails, application cannot be placed before the end of bootloader.


# LKExp-mbed-simple_bootloader

## Goal

Create an application starter, that will launch the main application

## Ressources

GitHub: [Mbed OS bootloader example](https://github.com/ARMmbed/mbed-os-example-bootloader)

Mbed website: [Creating and using a bootloader](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/creating-and-using-a-bootloader.html)

Mbed website: [Bootloader configuration](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/bootloader-configuration.html)

Mbed forum: [Bootloader tutorial not working with certain targets](https://forums.mbed.com/t/bootloader-tutorial-not-working-with-certain-targets/9104)

## Preparation

First you need to define the maximum bootloader size by setting the value of `target.restrict_size` in the `target_overrides` section in `mbed_app.json`:
```
"target_overrides": {
	"*": {
		"target.restrict_size": "0x10000",
		...
	}
```
This allows you to use the `POST_APPLICATION_ADDR` parameter corresponding to the bootloader ending address, which you can use to launch your main application by calling `mbed_start_application(POST_APPLICATION_ADDR);` at the end of the bootloader main function.

Build the bootloader application with the command `mbed compile` and find `<project_name>.bin` in `./BUILD/DISCO_F769NI/GCC_ARM/`, that is the binary image of your bootloader.

## Usage

In a project you want to use the bootloader with, create a folder named `bootloader` and place the bootloader binary image in. Rename it `bootloader.bin` for simplicity.

In the file `mbed_app.json` of this project, add the path of the bootloader image by setting `target.bootloader_img` in the `target_overrides` section:
```
"target_overrides": {
	"*": {
		"target.bootloader_img": "bootloader/bootloader.bin",
		...
	}
```

Make sure the DISCO_F769NI board is connected to your PC over USB and detected. Use the command `mbed compile -f` to build the application with the bootloader and flash it to the board. You can then see your main application run after the bootloader.

## Parameters and functions

### 1. In the bootloader project

#### 1.1. In `mbed_app.json`

The following parameter should be written in the `"target_overrides"` section.

`"target.restrict_size"`: sets the maximum size of the bootloader image. It defines the symbols `APPLICATION_ADDR`, `APPLICATION_SIZE`, `POST_APPLICATION_ADDR` and `POST_APPLICATION_SIZE`, making the ROM look like this:
```
|-------------------|   POST_APPLICATION_ADDR + POST_APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|                   |
|     Reserved      |
|                   |
+-------------------+   POST_APPLICATION_ADDR == APPLICATION_ADDR + APPLICATION_SIZE
|                   |
|    Application    |
|  (the bootloader) |
|                   |
+-------------------+   APPLICATION_ADDR == Start of ROM
```
The value we write for this parameter is directly given to `APPLICATION_SIZE`.

#### 1.2. In `src/main.cpp`

`mbed_start_application(POST_APPLICATION_ADDR)`: starts the application at the address `POST_APPLICATION_ADDR`, corresponding to the end of the bootloader memory.

### 2. In the main application project

#### In `mbed_app.json`

The following parameters should be written in the `"target_overrides"` section.

-	`"target.bootloader_img"`: path to the bootloader binary image. It redefines the previous symbols as `BOOTLOADER_ADDR`, `BOOTLOADER_SIZE`, `APPLICATION_ADDR` and `APPLICATION_SIZE`. The ROM now looks like this:
```
|-------------------|   APPLICATION_ADDR + APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|    Application    |
|   (main program)  |
|                   |
+-------------------+   APPLICATION_ADDR == BOOTLOADER_ADDR + BOOTLOADER_SIZE
|                   |
|    Bootloader     |
|    (<path to      |
|  bootloader bin>) |
|                   |
+-------------------+   BOOTLOADER_ADDR == Start of ROM
```

-	`"target.mbed_app_start"` or `"target.app_offset"`: sets the start address of the main application, creating space between the bootloader and the application. Be careful to set a value that does not get in conflict with the memory taken by the bootloader (`"target.restrict_size"`). Note that `"target.app_offset"` is relative to the start of ROM, which is not at the address `0x0`.

-	`"target.mbed_app_size"`: sets the maximum size of the application. In conjunction with the previous parameter, it also sets the end address of the application.

## Tests results

### Basic test

Test conditions:

-	`"target.restrict_size": "0x10000"` in `mbed_app.json` in bootloader project
-	Main application: prints "Hello World" every second

Results:

-	Application runs right after the bootloader
-	Bootloader starts at address `0x8000000` and application at address `0x8010000`

### Modifying the parameters

Tests conditions: main application prints "Hello World" every second

#### 1. `"target.restrict_size"`

Results:

-	With `"target.restrict_size"` between `0x8000` and `0xFFFF`
	-	`APPLICATION_SIZE` is `0x8000`, `POST_APPLICATION_ADDR` is `0x8008000`
	-	Compile fails, bootloader does not fit in allocated ROM (needs `0x8A64` bytes)
	
-	With `"target.restrict_size"` between `0x10000` and `0x1FFFF`
	-	`APPLICATION_SIZE` is `0x10000`, `POST_APPLICATION_ADDR` is `0x8010000`
	-	Compiler succeeds, application runs right after the bootloader
	
-	With `"target.restrict_size"` between `0x20000` and `0x3FFFF`
	-	`APPLICATION_SIZE` is `0x20000`, `POST_APPLICATION_ADDR` is `0x8020000`
	-	Compiler succeeds, bootloader runs but not the application (also with `"target.restrict_size"` above `0x40000`)

#### 2. `"target.restrict_size"` and `"target.app_offset"`

Results:

-	With `"target.restrict_size": "0x10000"` and `"target.app_offset": "0x10000"` (or whatever value with **`"target.restrict_size"`==`"target.app_offset"`**)
	-	`BOOTLOADER_SIZE` is the value given to `"target.restrict_size"` and `"target.app_offset"`
	-	Compiler succeeds, application runs right after the bootloader

-	With `"target.restrict_size"` < `"target.app_offset"`
	-	`BOOTLOADER_SIZE` is `0x10000`, `APPLICATION_ADDR` is the value given to `"target.app_offset"`
	-	Compiler succeeds, bootloader runs but not the application

-	With `"target.restrict_size"` > `"target.app_offset"`
	-	Compiler fails, application cannot be placed before the end of bootloader.




# LKExp-mbed-simple_bootloader

## Goal

Create an application starter, that will launch the main application

## Ressources

GitHub: [Mbed OS bootloader example](https://github.com/ARMmbed/mbed-os-example-bootloader)

Mbed website: [Creating and using a bootloader](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/creating-and-using-a-bootloader.html)

Mbed website: [Bootloader configuration](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/bootloader-configuration.html)

Mbed forum: [Bootloader tutorial not working with certain targets](https://forums.mbed.com/t/bootloader-tutorial-not-working-with-certain-targets/9104)

## Preparation

First you need to define the maximum bootloader size by setting the value of `target.restrict_size` in the `target_overrides` section in `mbed_app.json`:
```
"target_overrides": {
	"*": {
		"target.restrict_size": "0x10000",
		...
	}
```
This allows you to use the `POST_APPLICATION_ADDR` parameter corresponding to the bootloader ending address, which you can use to launch your main application by calling `mbed_start_application(POST_APPLICATION_ADDR);` at the end of the bootloader main function.

Build the bootloader application with the command `mbed compile` and find `<project_name>.bin` in `./BUILD/DISCO_F769NI/GCC_ARM/`, that is the binary image of your bootloader.

## Usage

In a project you want to use the bootloader with, create a folder named `bootloader` and place the bootloader binary image in. Rename it `bootloader.bin` for simplicity.

In the file `mbed_app.json` of this project, add the path of the bootloader image by setting `target.bootloader_img` in the `target_overrides` section:
```
"target_overrides": {
	"*": {
		"target.bootloader_img": "bootloader/bootloader.bin",
		...
	}
```

Make sure the DISCO_F769NI board is connected to your PC over USB and detected. Use the command `mbed compile -f` to build the application with the bootloader and flash it to the board. You can then see your main application run after the bootloader.

## Parameters and functions

### 1. In the bootloader project

#### 1.1. In `mbed_app.json`

The following parameter should be written in the `"target_overrides"` section.

`"target.restrict_size"`: sets the maximum size of the bootloader image. It defines the symbols `APPLICATION_ADDR`, `APPLICATION_SIZE`, `POST_APPLICATION_ADDR` and `POST_APPLICATION_SIZE`, making the ROM look like this:
```
|-------------------|   POST_APPLICATION_ADDR + POST_APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|                   |
|     Reserved      |
|                   |
+-------------------+   POST_APPLICATION_ADDR == APPLICATION_ADDR + APPLICATION_SIZE
|                   |
|    Application    |
|  (the bootloader) |
|                   |
+-------------------+   APPLICATION_ADDR == Start of ROM
```
The value we write for this parameter is directly given to `APPLICATION_SIZE`.

#### 1.2. In `src/main.cpp`

`mbed_start_application(POST_APPLICATION_ADDR)`: starts the application at the address `POST_APPLICATION_ADDR`, corresponding to the end of the bootloader memory.

### 2. In the main application project

#### In `mbed_app.json`

The following parameters should be written in the `"target_overrides"` section.

-	`"target.bootloader_img"`: path to the bootloader binary image. It redefines the previous symbols as `BOOTLOADER_ADDR`, `BOOTLOADER_SIZE`, `APPLICATION_ADDR` and `APPLICATION_SIZE`. The ROM now looks like this:
```
|-------------------|   APPLICATION_ADDR + APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|    Application    |
|   (main program)  |
|                   |
+-------------------+   APPLICATION_ADDR == BOOTLOADER_ADDR + BOOTLOADER_SIZE
|                   |
|    Bootloader     |
|    (<path to      |
|  bootloader bin>) |
|                   |
+-------------------+   BOOTLOADER_ADDR == Start of ROM
```

-	`"target.mbed_app_start"` or `"target.app_offset"`: sets the start address of the main application, creating space between the bootloader and the application. Be careful to set a value that does not get in conflict with the memory taken by the bootloader (`"target.restrict_size"`). Note that `"target.app_offset"` is relative to the start of ROM, which is not at the address `0x0`.

-	`"target.mbed_app_size"`: sets the maximum size of the application. In conjunction with the previous parameter, it also sets the end address of the application.

## Tests results

### Basic test

Test conditions:

-	`"target.restrict_size": "0x10000"` in `mbed_app.json` in bootloader project
-	Main application: prints "Hello World" every second

Results:

-	Application runs right after the bootloader
-	Bootloader starts at address `0x8000000` and application at address `0x8010000`

### Modifying the parameters

Tests conditions: main application prints "Hello World" every second

#### 1. `"target.restrict_size"`

Results:

-	With `"target.restrict_size"` between `0x8000` and `0xFFFF`
	-	`APPLICATION_SIZE` is `0x8000`, `POST_APPLICATION_ADDR` is `0x8008000`
	-	Compile fails, bootloader does not fit in allocated ROM (needs `0x8A64` bytes)
	
-	With `"target.restrict_size"` between `0x10000` and `0x1FFFF`
	-	`APPLICATION_SIZE` is `0x10000`, `POST_APPLICATION_ADDR` is `0x8010000`
	-	Compiler succeeds, application runs right after the bootloader
	
-	With `"target.restrict_size"` between `0x20000` and `0x3FFFF`
	-	`APPLICATION_SIZE` is `0x20000`, `POST_APPLICATION_ADDR` is `0x8020000`
	-	Compiler succeeds, bootloader runs but not the application (also with `"target.restrict_size"` above `0x40000`)

#### 2. `"target.restrict_size"` and `"target.app_offset"`

Results:

-	With `"target.restrict_size": "0x10000"` and `"target.app_offset": "0x10000"` (or whatever value with **`"target.restrict_size"`==`"target.app_offset"`**)
	-	`BOOTLOADER_SIZE` is the value given to `"target.restrict_size"` and `"target.app_offset"`
	-	Compiler succeeds, application runs right after the bootloader

-	With `"target.restrict_size"` < `"target.app_offset"`
	-	`BOOTLOADER_SIZE` is `0x10000`, `APPLICATION_ADDR` is the value given to `"target.app_offset"`
	-	Compiler succeeds, bootloader runs but not the application

-	With `"target.restrict_size"` > `"target.app_offset"`
	-	Compiler fails, application cannot be placed before the end of bootloader.


# LKExp-mbed-simple_bootloader

## Goal

Create an application starter, that will launch the main application

## Ressources

GitHub: [Mbed OS bootloader example](https://github.com/ARMmbed/mbed-os-example-bootloader)

Mbed website: [Creating and using a bootloader](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/creating-and-using-a-bootloader.html)

Mbed website: [Bootloader configuration](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/bootloader-configuration.html)

Mbed forum: [Bootloader tutorial not working with certain targets](https://forums.mbed.com/t/bootloader-tutorial-not-working-with-certain-targets/9104)

## Preparation

First you need to define the maximum bootloader size by setting the value of `target.restrict_size` in the `target_overrides` section in `mbed_app.json`:
```
"target_overrides": {
	"*": {
		"target.restrict_size": "0x10000",
		...
	}
```
This allows you to use the `POST_APPLICATION_ADDR` parameter corresponding to the bootloader ending address, which you can use to launch your main application by calling `mbed_start_application(POST_APPLICATION_ADDR);` at the end of the bootloader main function.

Build the bootloader application with the command `mbed compile` and find `<project_name>.bin` in `./BUILD/DISCO_F769NI/GCC_ARM/`, that is the binary image of your bootloader.

## Usage

In a project you want to use the bootloader with, create a folder named `bootloader` and place the bootloader binary image in. Rename it `bootloader.bin` for simplicity.

In the file `mbed_app.json` of this project, add the path of the bootloader image by setting `target.bootloader_img` in the `target_overrides` section:
```
"target_overrides": {
	"*": {
		"target.bootloader_img": "bootloader/bootloader.bin",
		...
	}
```

Make sure the DISCO_F769NI board is connected to your PC over USB and detected. Use the command `mbed compile -f` to build the application with the bootloader and flash it to the board. You can then see your main application run after the bootloader.

## Parameters and functions

### 1. In the bootloader project

#### 1.1. In `mbed_app.json`

The following parameter should be written in the `"target_overrides"` section.

`"target.restrict_size"`: sets the maximum size of the bootloader image. It defines the symbols `APPLICATION_ADDR`, `APPLICATION_SIZE`, `POST_APPLICATION_ADDR` and `POST_APPLICATION_SIZE`, making the ROM look like this:
```
|-------------------|   POST_APPLICATION_ADDR + POST_APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|                   |
|     Reserved      |
|                   |
+-------------------+   POST_APPLICATION_ADDR == APPLICATION_ADDR + APPLICATION_SIZE
|                   |
|    Application    |
|  (the bootloader) |
|                   |
+-------------------+   APPLICATION_ADDR == Start of ROM
```
The value we write for this parameter is directly given to `APPLICATION_SIZE`.

#### 1.2. In `src/main.cpp`

`mbed_start_application(POST_APPLICATION_ADDR)`: starts the application at the address `POST_APPLICATION_ADDR`, corresponding to the end of the bootloader memory.

### 2. In the main application project

#### In `mbed_app.json`

The following parameters should be written in the `"target_overrides"` section.

-	`"target.bootloader_img"`: path to the bootloader binary image. It redefines the previous symbols as `BOOTLOADER_ADDR`, `BOOTLOADER_SIZE`, `APPLICATION_ADDR` and `APPLICATION_SIZE`. The ROM now looks like this:
```
|-------------------|   APPLICATION_ADDR + APPLICATION_SIZE == End of ROM
|                   |
...
|                   |
|    Application    |
|   (main program)  |
|                   |
+-------------------+   APPLICATION_ADDR == BOOTLOADER_ADDR + BOOTLOADER_SIZE
|                   |
|    Bootloader     |
|    (<path to      |
|  bootloader bin>) |
|                   |
+-------------------+   BOOTLOADER_ADDR == Start of ROM
```

-	`"target.mbed_app_start"` or `"target.app_offset"`: sets the start address of the main application, creating space between the bootloader and the application. Be careful to set a value that does not get in conflict with the memory taken by the bootloader (`"target.restrict_size"`). Note that `"target.app_offset"` is relative to the start of ROM, which is not at the address `0x0`.

-	`"target.mbed_app_size"`: sets the maximum size of the application. In conjunction with the previous parameter, it also sets the end address of the application.

## Tests results

### Basic test

Test conditions:

-	`"target.restrict_size": "0x10000"` in `mbed_app.json` in bootloader project
-	Main application: prints "Hello World" every second

Results:

-	Application runs right after the bootloader
-	Bootloader starts at address `0x8000000` and application at address `0x8010000`

### Modifying the parameters

Tests conditions: main application prints "Hello World" every second

#### 1. `"target.restrict_size"`

Results:

-	With `"target.restrict_size"` between `0x8000` and `0xFFFF`
	-	`APPLICATION_SIZE` is `0x8000`, `POST_APPLICATION_ADDR` is `0x8008000`
	-	Compile fails, bootloader does not fit in allocated ROM (needs `0x8A64` bytes)
	
-	With `"target.restrict_size"` between `0x10000` and `0x1FFFF`
	-	`APPLICATION_SIZE` is `0x10000`, `POST_APPLICATION_ADDR` is `0x8010000`
	-	Compiler succeeds, application runs right after the bootloader
	
-	With `"target.restrict_size"` between `0x20000` and `0x3FFFF`
	-	`APPLICATION_SIZE` is `0x20000`, `POST_APPLICATION_ADDR` is `0x8020000`
	-	Compiler succeeds, bootloader runs but not the application (also with `"target.restrict_size"` above `0x40000`)

#### 2. `"target.restrict_size"` and `"target.app_offset"`

Results:

-	With `"target.restrict_size": "0x10000"` and `"target.app_offset": "0x10000"` (or whatever value with **`"target.restrict_size"`==`"target.app_offset"`**)
	-	`BOOTLOADER_SIZE` is the value given to `"target.restrict_size"` and `"target.app_offset"`
	-	Compiler succeeds, application runs right after the bootloader

-	With `"target.restrict_size"` < `"target.app_offset"`
	-	`BOOTLOADER_SIZE` is `0x10000`, `APPLICATION_ADDR` is the value given to `"target.app_offset"`
	-	Compiler succeeds, bootloader runs but not the application

-	With `"target.restrict_size"` > `"target.app_offset"`
	-	Compiler fails, application cannot be placed before the end of bootloader.
