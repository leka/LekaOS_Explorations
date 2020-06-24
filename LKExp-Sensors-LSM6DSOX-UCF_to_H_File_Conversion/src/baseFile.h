#ifndef BASE_FILE_H
#define BASE_FILE_H

#include <string>

using namespace std;
string _fileHead = "/*\n* ###########\n* Intro here\n* ###########\n/*\n\n";

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

string _ucfLineFormat = "\t{.address = 0x%s, .data = 0x%s,},\n";

string _endOfFile = 
"};\n"
"\n"
"#ifdef __cplusplus\n"
"}\n"
"#endif\n"
"\n"
"#endif /* MLC_CONFIG_H */\n";

#endif /* BASE_FILE_H */