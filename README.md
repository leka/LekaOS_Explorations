# LekaOS Explorations

## About

This repository contains all our experimentations with [Arm Mbed OS](https://github.com/armmbed/mbed-os) & [STM32CubeF7](https://github.com/STMicroelectronics/STM32CubeF7).

This is a *quick & dirty* repository whose main goal is to test a lot of things, not lose anything, share issues and bugs before merging in our main LekaOS repository.

No branches, few PRs, each test/example in its own directory.

## How to use

### With mbed

Each time you want to add a new experimentation, you need to do the following steps:

- (clone this repository if not already)
- copy the `_template` folder and git it a meaningful name that starts with `LKExp-mbed-`
- `cd` in the new directory and `mbed deploy --depth 1`

```bash
# copy _template
$ cp -r _template LKExp-mbed-ST_play_video_from_sd

# cd to the new directory
$ cd LKExp-ST_play_video_from_sd

# pull mbed os if needed
$ mbed deploy --depth 1

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
