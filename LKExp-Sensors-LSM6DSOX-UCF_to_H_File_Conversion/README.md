# LKExp-Sensors-UCF_to_H_file_conversion

Small program used to transform a _.ucf_ file in the adequate _.h_ file that can be used to configure the LSM6DSOX MLC

__User manual :__

- Call the makefile in the /src folder in order to compile and build the executable.
- Launch the executable ("ucf-to-h") with `$ ./ucf-to-h`
- Possible arguments
	-  __-h__ or __--help__ Help message with this information
    -  __-i \<input_file\>__ Input ucf file to convert
    -  __-o \<output_file\>__ Output file with the adequate format
	-  __--overwrite__ If the output file already exists, it will be writen over without a warning

⚠ If no input or output files are passed as argument, the program will use the default ones (use _--help_ to know their name)