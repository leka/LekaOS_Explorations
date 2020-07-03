# LKExp-Sensors-LSM6DSOX-UCF_Analysis

Small executable used to comment a UCF file based on the documented registers. this aims to ease the ucf comprehension.

⚠️ This project does not work in it's actual state.
A lot of logic of the UCF is not implemented (registers accessed when page sel, etc).
The idea was just to give an idea of what could be made in order to simplify UCF analysis.

We are still waiting for STMicroelectronics to give more documentation about the register pages accessed when configuring the LSM6DSOX **Machine Learning Core** as this is the principal piece of missing information right now.

Some of the next steps of this project can be found in the [ToDo](todo.md) file of this project, it's a good starting point to continue working on it.

## How does it work?
The code reads the CSV files for each register map and creates a dictionnary where the key is the register address.

Once all the logic about the LSM6DSOX register selection is implemented, this project could be able to take as input an UCF file and get rid of redundant commands as well as comment it, allowing easier comprehension.

You can check the [commented file](commented.txt) to get an idea of what could be done.