### Intro
This is a small step by step to configure linux in order to be able to work on Leka projects.
These steps work on Ubuntu 18.04.
A lot of info was already given in the [bare_mbed_project](https://github.com/ladislas/Bare-mbed-Project).
This is just a more detailed version aimed at the total beginners we are.
It also contains some tweaks and common errors we stumbled upon.


### Install Mbed CLI: 
https://os.mbed.com/docs/mbed-os/v6.0/build-tools/install-and-set-up.html
(parts 1 and 2)

Install python, git, mercurial
`$ sudo apt install python3 python3-pip git mercurial`

install mbed cli
`$ python3 -m pip install mbed-cli`

test mbed-cli install
`$ mbed --help`

if the command is not found, a restart might be needed

update mbed cli
`$ python3 -m pip install -U mbed-cli`

### Install ARM Toolchain : 
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

it's a tar.bz2 file, inside there is a readme.txt in  `share/doc/gcc-arm-none-eabi`

* Installing executables on Linux
Unpack the tarball to the install directory, like this:
    `$ cd "your install directory" && tar xjf gcc-arm-none-eabi-_version_-linux.tar.bz2`

* Invoking GCC
On Linux and Mac OS X, either invoke with the complete path like this:
    `$ "your install directory"/gcc-arm-none-eabi-*/bin/arm-none-eabi-gcc`

Or set path like this:

copy line at the end of ~/.profile 
`$ export PATH=$PATH:"'your install directory'/gcc-arm-none-eabi-*/bin"`

test _arm-none-eabi-gcc_ install
`$ arm-none-eabi-gcc --version`


### Import project

    $ git clone https://github.com/ladislas/Bare-mbed-Project my-project-name
    $ cd my-project-name
    $ git clone --depth=1 https://github.com/ARMmbed/mbed-os ./lib/mbed-os 


for Leka OS Exploration
\## git checkout -b {user}-{Topic}-{(optional) Hardware}-{The_Goal_Of_The_Experiment}
    `$ git checkout -b ladislas-Sensors-LSM6DSOX-Drivers_Comparision`

Copy _template and give your experiment a meaningful name with the following pattern:
    LKExp-{Topic}-{(optional) Hardware}-{The_Goal_Of_The_Experiment}
    
Check the other directories for inspiration
    `$ cp -r _template LKExp-Sensors-LSM6DSOX-Drivers_Comparision`

cd to the new directory
    `$ cd LKExp-Sensors-LSM6DSOX-Drivers_Comparision`

clone mbed os lib
    `$ git clone --depth=1 https://github.com/ARMmbed/mbed-os ./lib/mbed-os`


then when you need to compile and flash
    `$ mbed compile --flash`
or (for the WB55RG board)
    `$ mbed compile -m NUCLEO_WB55RG -f`
    
and then using serial term 
    `$ mbed sterm `

if it says that it's opening the term but exits immediately, you need to add you user to the group __dialout__ (grants access to serial)
    `$ sudo adduser [username] dialout`


---
__OLD FIXES__ (this seems to be irrelevant if adding user to _dialout_ user group)
if it says that it's opening the term but exits immediately, you need sudo
    `$ sudo mbed sterm`

if the command is not found, try using the actual evironment path for sudo
    `$ sudo -E env "PATH=$PATH" <command> [arguments]`


you can make it into an alias for easo of use
    `$ alias mysudo='sudo -E env "PATH=$PATH"'`

and then 
    `$ mysudo <command> [arguments]`

the alias can be added to ./bashrc in order to save it for the next time 
    `$ nano ~./bashrc`
    scroll to the end of the file and add
    #alias to use sudo with user path, solves some "command not found problems"
    `alias mysudo='sudo -E env "PATH=$PATH"'`
    then save
