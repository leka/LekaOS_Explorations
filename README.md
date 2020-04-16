# LekaOS Explorations

## About

This repository contains all our experimentations with [Arm Mbed OS](https://github.com/armmbed/mbed-os) & [STM32CubeF7](https://github.com/STMicroelectronics/STM32CubeF7).

This is a *quick & dirty* repository whose main goal is to test a lot of things, not lose anything, share issues and bugs before merging in our main LekaOS repository.

No branches, few PRs, each test/example in its own directory.

## How to use

Each time you want to add a new experimentation, you need to do the following steps:

- clone this repository if not already
- create a new branch named with this pattern:

```bash
# git checkout -b {user}-{mbed or not}-{what is being tested}
$ git checkout -b ladislas-mbed-st_play_video_from_sd
# or
$ git checkout -b ladislas-st_display_image_from_sram
```

### With mbed

If you are using mbed:

- copy the `_template` folder and git it a meaningful name that starts with `LKExp-mbed-`
- `cd` in the new directory and `mbed deploy --depth=1`

```bash
# copy _template
$ cp -r _template LKExp-mbed-ST_play_video_from_sd

# cd to the new directory
$ cd LKExp-ST_play_video_from_sd

# pull mbed os 
$ git clone --depth=1 https://github.com/ARMmbed/mbed-os ./lib/mbed-os

# then when you need to compile and flash
$ mbed compile --flash
```

### Without mbed

In case you are no using mbed:

- you **don't need** to copy `_template`
- create your own directory starting with just `LKExp-`
- add all the files necessary, even a complete STMCubeIDE project if needed
- the project must be self contained as much as possible
- add a `README.md` with specific instructions if needed (keep it short)
- use a Makefile if possible
 
## Resources

- [ladislas/Bare-mbed-Project](https://github.com/ladislas/Bare-mbed-Project)
- [STMicroelectronics/STM32CubeF7](https://github.com/STMicroelectronics/STM32CubeF7)
- [armmbed/mbed-os](https://github.com/armmbed/mbed-os)

## License

Copyright (c) APF France handicap & Ladislas de Toldi. All rights reserved.

Licensed under the Apache-2.0 license.
