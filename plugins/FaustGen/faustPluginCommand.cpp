#include "faustPluginCommand.hpp"

namespace FaustGen {

extern const bool debug_messages;

FaustData faustData;

/**********************************************
 *
 * PARSER
 *
 * *******************************************/
void printDSPInfo(dsp *dsp) {
  std::cout << "Num faust DSP inputs:" << dsp->getNumInputs() << std::endl
            << "Num faust DSP outputs: " << dsp->getNumOutputs() << std::endl
            << "Faust DSP Samplerate: " << dsp->getSampleRate() << std::endl;
  /* << "Num UGen inputs: " << this->mNumInputs << std::endl */
  /* << "UGen audio inputs: " << mNumAudioInputs << std::endl */
  /* << "Num UGen outputs: " << this->mNumOutputs << std::endl; */
}

// Initial parsing stage: Try to create dsp factory from the code supplied
bool parse(FaustCommandData *cmdData) {

  // Create factory
  auto optimize = -1;
  auto argc = 0;
  const char **argv = NULL;
  auto name = "faustgen";
  std::string errorString;
  cmdData->factory = createDSPFactoryFromString(name, cmdData->code, argc, argv,
                                                "", errorString, optimize);

  // If a factory cannot be created it is usually because of (syntax) errors in
  // the faust code
  if (!cmdData->factory) {

    std::cout << errorString << std::endl;
    return false;
  } else {
    // Post debug info
    if (debug_messages) {
      Print("Created faust factory \n");

      const auto dspcode = cmdData->factory->getDSPCode();

      std::cout << dspcode << std::endl;

      std::cout << "Compile options: \n"
                << cmdData->factory->getCompileOptions() << std::endl;

      Print("Include path names: \n");
      for (auto path : cmdData->factory->getIncludePathnames())
        std::cout << "\t" << path << std::endl;

      Print("Library list: \n");
      for (auto libb : cmdData->factory->getLibraryList())
        std::cout << "\t" << libb << std::endl;

      std::cout << cmdData->factory->getTarget() << std::endl;
    }

    return true;
  }
}

/**********************************************
 *
 * PLUGIN COMMUNICATION
 *
 * *******************************************/

// PARSE CODE AND CREATE DSP
// stage2 is non real time
bool cmdStage2(World *world, void *inUserData) {
  FaustCommandData *faustCmdData = (FaustCommandData *)inUserData;

  faustCmdData->parsedOK = parse(faustCmdData);

  // creating the DSP instance for interfacing
  if (faustCmdData->parsedOK) {
    faustCmdData->commandDsp = faustCmdData->factory->createDSPInstance();
    faustCmdData->commandDsp->init(static_cast<int>(faustCmdData->sampleRate));

    return true;
  } else {
    return false;
  }
}

// PASS DSP TO UNIT
// stage3 is real time - completion msg performed if stage3 returns true
bool cmdStage3(World *world, void *inUserData) {
  FaustCommandData *faustCmdData = (FaustCommandData *)inUserData;

  auto thisId = faustCmdData->id;
  auto instance = faustData.instances.at(thisId);
  if (instance) {
    instance->setNewDSP(faustCmdData->commandDsp);
    return true;
  } else {
    return false;
  }
}

// stage4 is non real time - sends done if stage4 returns true
bool cmdStage4(World *world, void *inUserData) { return true; }

void cmdCleanup(World *world, void *inUserData) {
  FaustCommandData *faustCmdData = (FaustCommandData *)inUserData;

  RTFree(world, faustCmdData->code); // free the string
  // @TODO will this delete factory and dsp as well, properly?
  RTFree(world, faustCmdData); // free command data
  // scsynth will delete the completion message for you.
}

void receiveNewFaustCode(World *inWorld, void *inUserData,
                         struct sc_msg_iter *args, void *replyAddr) {

  // allocate command data, free it in cmdCleanup.
  FaustCommandData *faustCmdData =
      (FaustCommandData *)RTAlloc(inWorld, sizeof(FaustCommandData));

  faustCmdData->sampleRate = inWorld->mSampleRate;

  // ID arguments
  faustCmdData->id = args->geti();
  faustCmdData->nodeID = args->geti();

  const char *newCode = args->gets(); // get the string argument
  if (newCode) {
    faustCmdData->code = (char *)RTAlloc(inWorld, strlen(newCode) + 1);
    // allocate space, free it in cmdCleanup. */
    strcpy(faustCmdData->code, newCode);

    std::cout << "Received new code: " << newCode << std::endl;
  }

  // how to pass a completion message
  int msgSize = args->getbsize();
  char *msgData = 0;
  if (msgSize) {
    // allocate space for completion message scsynth will delete the completion
    // message for you.
    msgData = (char *)RTAlloc(inWorld, msgSize);
    // copy completion message.
    args->getb(msgData, msgSize);
  }

  DoAsynchronousCommand(inWorld, replyAddr, "fausteval", (void *)faustCmdData,
                        (AsyncStageFn)cmdStage2, (AsyncStageFn)cmdStage3,
                        (AsyncStageFn)cmdStage4, cmdCleanup, msgSize, msgData);
}

} // namespace FaustGen
