all: compile

compile:
	mbed compile --color --profile=debug --profile=./leka_cpp.json

flash:
	mbed compile --color --profile=debug --flash --profile=./leka_cpp.json

term:
	mbed sterm

wb55:
	mbed compile -m NUCLEO_WB55RG --color --profile=debug --profile=./leka_cpp.json -f

vs:
	mv Makefile tmpMakefile
	mbed-vscode-generator -m disco_f769ni -m NUCLEO_WB55RG
	mv tmpMakefile Makefile
