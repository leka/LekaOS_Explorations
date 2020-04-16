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
# git checkout -b {user}-{Topic}-{(optional) Hardware}-{The_Goal_Of_The_Experiment}
$ git checkout -b ladislas-Sensors-LSM6DSOX-Drivers_Comparision
# or
$ git checkout -b ladislas-Sensors-Nucleo_IKS01A2-Mbed_ST_Drivers
```

⚠️ When you're done (*or even while working*), `git push` your branch to origin and then open a PR for others to review.

### With mbed

If you are using mbed:

```bash
# copy _template and give your experiment a meaningful name with the following pattern:
# LKExp-{Topic}-{(optional) Hardware}-{The_Goal_Of_The_Experiment}
# Check the other directories for inspiration
$ cp -r _template LKExp-Sensors-LSM6DSOX-Drivers_Comparision

# cd to the new directory
$ cd LKExp-Sensors-LSM6DSOX-Drivers_Comparision

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
