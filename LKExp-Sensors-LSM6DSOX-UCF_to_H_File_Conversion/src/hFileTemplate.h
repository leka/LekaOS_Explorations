/**
 * @file hFileTemplate.h
 * @author Maxime BLANC & Samuel HADJES
 * @brief Template strings to generate the .h file used to configure the LSM6DSOX Machine Learning Core
 * @date 2020-06-30
 */

#ifndef H_FILE_TEMPLATE_H
#define H_FILE_TEMPLATE_H

#include <string>

using namespace std;
/**
 * @brief String placed at the top of the header file generated, with some infos on the file
 * 
 */
string _fileHead = "/*\n* ###########\n* Header file generated from a .ucf by UCF_To_H_File_Conversion, is to be included in an mbed project instead of the usual Unico Configuration File \n* ###########\n/*\n\n";


/**
 * @brief String of the first lines of the header that come before the register writing
 * 
 */
string _startOfFile = 
"/* Define to prevent recursive inclusion -------------------------------------*/\n"
"#ifndef MLC_CONFIG_H\n"
"#define MLC_CONFIG_H\n"
"\n"
"#ifdef __cplusplus\n"
"  extern \"C\" {\n"
"#endif\n"
"\n"
"/* Includes ------------------------------------------------------------------*/\n"
"#include <stdint.h>\n"
"#ifndef MEMS_UCF_SHARED_TYPES\n"
"#define MEMS_UCF_SHARED_TYPES\n"
"\n"
"/** Common data block definition **/\n"
"typedef struct {\n"
"  uint8_t address;\n"
"  uint8_t data;\n"
"} ucf_line_t;\n"
"\n"
"#endif /* MEMS_UCF_SHARED_TYPES */\n"
"\n\n"
"const ucf_line_t lsm6dsox_mlc_configuration[] = {\n";


/**
 * @brief String showing how each line corresponding to a register data writing has to be formated
 * 
 */
string _ucfLineFormat = "\t{.address = 0x%s, .data = 0x%s,},\n";


/**
 * @brief String of the last lines of the header that come after the register writing
 * 
 */
string _endOfFile = 
"};\n"
"\n"
"#ifdef __cplusplus\n"
"}\n"
"#endif\n"
"\n"
"#endif /* MLC_CONFIG_H */\n";

#endif /* H_FILE_TEMPLATE_H */