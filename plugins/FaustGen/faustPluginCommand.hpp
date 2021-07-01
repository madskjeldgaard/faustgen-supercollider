#pragma once

#include "FaustGen.hpp"
#include "SC_PlugIn.hpp"
#include <iostream>
#include <string>

extern InterfaceTable *ft;

namespace FaustGen {

extern const bool debug_messages;

// Data structure
struct FaustData {
  std::unordered_map<int, FaustGen *> instances;
};

struct FaustCommandData {
  llvm_dsp_factory *factory;
  dsp *commandDsp;

  int id;
  // @TODO how to use nodeID to look up instances?
  int nodeID;

  char *code;

  double sampleRate;

  bool parsedOK;
};


/**********************************************
 *
 * PARSER
 *
 * *******************************************/
void printDSPInfo(dsp *dsp); 

// Initial parsing stage: Try to create dsp factory from the code supplied
bool parse(FaustCommandData *cmdData); 


/**********************************************
 *
 * PLUGIN COMMUNICATION
 *
 * *******************************************/

// PARSE CODE AND CREATE DSP
// stage2 is non real time
bool cmdStage2(World *world, void *inUserData); 

// PASS DSP TO UNIT
// stage3 is real time - completion msg performed if stage3 returns true
bool cmdStage3(World *world, void *inUserData); 

// stage4 is non real time - sends done if stage4 returns true
bool cmdStage4(World *world, void *inUserData); 

void cmdCleanup(World *world, void *inUserData); 

void receiveNewFaustCode(World *inWorld, void *inUserData,
                         struct sc_msg_iter *args, void *replyAddr); 

} // namespace FaustGen

