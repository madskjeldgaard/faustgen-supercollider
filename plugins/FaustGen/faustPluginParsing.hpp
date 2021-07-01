#pragma once
#ifndef FAUST_PARSER
#define FAUST_PARSER
#include <iostream>
#include <string>
#include "faustPluginCommand.hpp"

namespace FaustGen{

/**********************************************
 *
 * PARSER
 *
 * *******************************************/

void printDSPInfo(dsp *dsp); 

// Initial parsing stage: Try to create dsp factory from the code supplied
bool parse(FaustCommandData *cmdData); 

}

#endif
