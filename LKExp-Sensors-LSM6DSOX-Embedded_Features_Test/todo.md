## Embedded functions todo list

### Documentation
- [ ]  write readme
- [ ]  doxygen
- [ ]  maybe another markdown doc with info about embedded functions
    - [ ] related registers
    - [ ] list of functions
    - [ ] possibilities to work with sensors / mlc


### Code
- [ ] implement all embedded functions
    - [ ]  tilt
    - [ ]  pedometer
    - [ ]  wake up
    - [ ]  ...
- [ ]  test working with MLC
- [ ]  


#### Idea for code
small polling app that tells all the states
 

### Functions
* Event-detection interrupts (fully configurable)
    * [ ] Free-fall
    * [ ] 6D orientation
    * [ ] Click and double-click sensing
    * [ ] Activity/Inactivity recognition
    * [ ] Stationary/Motion detectio
* Spefic IP blocks with negligible power consumption and high-performanc
    * [ ] Pedometer functions: step detector and step counters
    * [ ] Tilt
    * [ ] Significant Motion Detection







# random info to insert in doc

### Tilt detection
The tilt function helps to detect activity change and has been implemented in hardware using only the
accelerometer to achieve targets of both ultra-low power consumption and robustness during the short duration of
dynamic accelerations.
The tilt function is based on a trigger of an event each time the device's tilt changes and can be used with
different scenarios, for example:
* Triggers when phone is in a front pants pocket and the user goes from sitting to standing or standing to
sitting;
* Doesn’t trigger when phone is in a front pants pocket and the user is walking, running or going upstairs.


### Significant Motion Detection
The Significant Motion Detection (SMD) function generates an interrupt when a ‘significant motion’, that could be
due to a change in user location, is detected. In the LSM6DSOX device this function has been implemented in
hardware using only the accelerometer.
SMD functionality can be used in location-based applications in order to receive a notification indicating when the
user is changing location.




### registers

#### Base registers
**FUNC_CFG_ACCESS** (01h) : Enable embedded functions register (r/w)
    * FUNC_CFG_ACCESS : Enable access to the embedded functions configuration registers




* **PAGE_SEL** (02h) : Enable advanced features dedicated page (r/w)
    * PAGE_SEL[3:0] : Select the advanced features dedicated page

* **EMB_FUNC_EN_A** (04h) : Embedded functions enable register (r/w)
    * SIGN_MOTION_EN : Enable significant motion detection function
    * TILT_EN : Enable tilt calculation
    * PEDO_EN : Enable pedometer algorithm

* **EMB_FUNC_INT1** (0Ah) : INT1 pin control register (r/w)
Each bit in this register enables a signal to be carried through INT1.
This bits are effective if the *INT1_EMB_FUNC* bit of **MD1_CFG** (5Eh) is set to 1.
    * INT1_SIG_MOT : Routing of significant motion event on INT1
    * INT1_TILT : Routing of tilt event on INT1
    * INT1_STEP_DETECTOR : Routing of pedometer step recognition event on INT1

* **EMB_FUNC_INT2** (0Eh) : INT2 pin control register (r/w)
Each bit in this register enables a signal to be carried through INT2. The pin's output will supply the OR combination of the selected signals
    * INT2_SIG_MOT : Routing of significant motion event on INT2
    * INT2_TILT : Routing of tilt event on INT2
    * INT2_STEP_DETECTOR : Routing of pedometer step recognition event on INT2

* **EMB_FUNC_STATUS** (12h) : Embedded function status register (r).
    * IS_SIGMOT : Interrupt status bit for significant motion detection
    * IS_TILT : Interrupt status bit for tilt detection
    * IS_STEP_DET : Interrupt status bit for step detection

* **PAGE_RW (17h)** : Enable read and write mode of advanced features dedicated page (r/w)
    * EMB_FUNC_LIR : Latched Interrupt mode for Embedded Functions

* **EMB_FUNC_FIFO_CFG** (44h) : Embedded functions batching configuration register (r/w)
    * PEDO_FIFO_EN : Enable FIFO batching of step counter values
    
* **STEP_COUNTER_L** (62h) and **STEP_COUNTER_H** (63h) : Step counter output register (r).
    * STEP_[7:0] : Step counter output (LSbyte)
    * STEP_[15:8] : Step counter output (MSbyte)

* **EMB_FUNC_SRC** (64h) : Embedded function source register (r/w)
    * PEDO_RST_STEP : Reset pedometer step counter. Read/write bit.
    * STEP_DETECTED : Step detector event detection status. Read-only bit.
    * STEP_COUNT_DELTA_IA : Pedometer step recognition on delta time status. Read-only bit.
    * STEP_OVERFLOW : Step counter overflow status. Read-only bit.
    * STEPCOUNTER_BIT_SET : This bit is equal to 1 when the step count is increased. If a timer period is programmed in **PEDO_SC_DELTAT_L** (D0h) and **PEDO_SC_DELTAT_H** (D1h) embedded advanced features (page 1) registers, this bit is kept to 0. Read-only bit.

* **EMB_FUNC_INIT_A** (66h) : Embedded functions initialization register (r/w)
    * SIG_MOT_INIT : Significant Motion Detection algorithm initialization request.
    * TILT_INIT : Tilt algorithm initialization request.
    * STEP_DET_INIT : Pedometer Step Counter/Detector algorithm initialization request.

* **MD1_CFG** (5Eh) : Functions routing on INT1 register (r/w)
    * INT1_SLEEP_CHANGE : Routing of activity/inactivity recognition event on INT1
    Activity/Inactivity interrupt mode (sleep change or sleep status) depends on the *SLEEP_STATUS_ON_INT* bit in **TAP_CFG0** (56h) register.
    * INT1_SINGLE_TAP : Routing of single-tap recognition event on INT1
    * INT1_WU : Routing of wakeup event on INT1
    * INT1_FF : Routing of free-fall event on INT1
    * INT1_DOUBLE_TAP : Routing of tap event on INT1
    * INT1_6D : Routing of 6D event on INT1
    * INT1_EMB_FUNC : Routing of embedded functions event on INT1

* **TAP_CFG0** (56h) : Activity/inactivity functions, configuration of filtering, and tap recognition functions (r/w).
    * INT_CLR_ON_READ : This bit allows immediately clearing the latched interrupts of an event detection upon the read of the corresponding status register. It must be set to 1 together with LIR
    * SLEEP_STATUS_ON_INT : Activity/inactivity interrupt mode configuration. If *INT1_SLEEP_CHANGE* or *INT2_SLEEP_CHANGE* bits are enabled, drives the sleep status or sleep change on the INT pins.
    * TAP_X_EN : Enable X direction in tap recognition.
    * TAP_Y_EN : Enable Y direction in tap recognition
    * TAP_Z_EN : Enable Z direction in tap recognition
    * LIR : Latched Interrupts on basic functions

* **PEDO_CMD_REG**
* **PEDO_DEB_STEPS_CONF**
* **PEDO_SC_DELTAT_L**
* **PEDO_SC_DELTAT_H**
