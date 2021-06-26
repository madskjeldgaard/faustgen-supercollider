// PluginFaustGen.cpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#include "FaustGen.hpp"
#include "SC_PlugIn.hpp"
#include <iostream>
#include <string>

static InterfaceTable *ft;

namespace FaustGen {
constexpr bool debug_messages = true;

// Data structure
struct FaustData {
  std::unordered_map<int, FaustGen *> instances;
  FaustGen *instance;
};

struct FaustCommandData {
  FaustData *pluginData;

  int id;
  // @TODO how to use nodeID to look up instances?
  int nodeID;

  char *code;
};

FaustData faustData;

/**********************************************
 *
 * PARSER
 *
 * *******************************************/

bool FaustGen::parse(char *theCode) {

  // Create factory
  auto optimize = -1;
  auto argc = 0;
  const char **argv = NULL;
  auto name = "faustgen";
  m_factory = createDSPFactoryFromString(name, theCode, argc, argv, "",
                                         m_errorString, optimize);

  // If a factory cannot be created it is usually because of (syntax) errors in
  // the faust code
  if (!m_factory) {

    std::cout << m_errorString << std::endl;
    return false;
  } else {
    // Post debug info
    if (debug_messages) {
      Print("Created faust factory \n");

      const auto dspcode = m_factory->getDSPCode();

      std::cout << dspcode << std::endl;

      std::cout << "Compile options: \n"
                << m_factory->getCompileOptions() << std::endl;

      Print("Include path names: \n");
      for (auto path : m_factory->getIncludePathnames())
        std::cout << "\t" << path << std::endl;

      Print("Library list: \n");
      for (auto libb : m_factory->getLibraryList())
        std::cout << "\t" << libb << std::endl;

      std::cout << m_factory->getTarget() << std::endl;
    }
  }

  // creating the DSP instance for interfacing
  m_dsp = m_factory->createDSPInstance();
  m_dsp->init(static_cast<int>(sampleRate()));

  if (!m_dsp) {
    Print("Could not create FAUST dsp \n");
    return false;

    // Check number of outputs to avoid crashing
  } else if (m_dsp->getNumOutputs() != this->mNumOutputs) {

    std::cout << "Error: Number of faust code outputs does not correspond to "
                 "UGen's number of outputs \n"
              << "Num UGEN outputs: " << this->mNumOutputs << std::endl;

    printDSPInfo();
    return false;

    // Check number of inputs to avoid crashing
  } else if (m_dsp->getNumInputs() > mNumAudioInputs) {

    std::cout << "Error: Number of faust code inputs does not correspond to "
                 "UGen's number of inputs \n"
              << "Num UGEN inputs: "
              << this->mNumInputs - InputName::NumParameters << std::endl;

    printDSPInfo();
    return false;
  } else {

    // Post debug info
    if (debug_messages) {
      printDSPInfo();
    }

    return true;
  };
}

void FaustGen::printDSPInfo() {
  std::cout << "Num faust DSP inputs:" << m_dsp->getNumInputs() << std::endl
            << "Num faust DSP outputs: " << m_dsp->getNumOutputs() << std::endl
            << "Faust DSP Samplerate: " << m_dsp->getSampleRate() << std::endl
            << "Num UGen inputs: " << this->mNumInputs << std::endl
            << "UGen audio inputs: " << mNumAudioInputs << std::endl
            << "Num UGen outputs: " << this->mNumOutputs << std::endl;
}

void FaustGen::evaluateCode(char *code) {
  // @TODO check if factory already exists, if it does do something that
  // allocates less
  bool result = parse(code);

  if (result) {
    mCalcFunc = make_calc_function<FaustGen, &FaustGen::next>();
  } else {
    // Setting clear function to be the calculation function when syntax or
    // other errors occur in the faust code interpretation
    mCalcFunc = make_calc_function<FaustGen, &FaustGen::clear>();
  }
};

/**********************************************
 *
 * UGen guts
 *
 * *******************************************/

FaustGen::FaustGen() {
  mNumAudioInputs = this->mNumInputs - InputName::NumParameters;

  // Insert instance into global data space
  id = static_cast<int>(in0(InputName::Id));
  std::pair<int, FaustGen *> instance(id, this);
  faustData.instances.insert(instance);
  mCalcFunc = make_calc_function<FaustGen, &FaustGen::clear>();
}

FaustGen::~FaustGen() {
  // cleaning
  delete m_dsp;
  /* delete m_ui; */
  deleteDSPFactory(m_factory);

  faustData.instances.erase(id);
}

void FaustGen::clear(int nSamples) { ClearUnitOutputs(this, nSamples); }
void FaustGen::next(int nSamples) {

  float **faustInputs;

  /* Remove inputs used by the UGen at init */
  for (size_t in_num; in_num < mNumAudioInputs; in_num++) {
    const auto offset = mNumAudioInputs;
    faustInputs[in_num] = this->mInBuf[in_num + offset];
  }

  // compute faust code
  m_dsp->compute(nSamples, (FAUSTFLOAT **)faustInputs,
                 (FAUSTFLOAT **)this->mOutBuf);
}

/**********************************************
 *
 * PLUGIN COMMUNICATION
 *
 * *******************************************/

// stage2 is non real time
bool cmdStage2(World *world, void *inUserData) {
  FaustCommandData *myCmdData = (FaustCommandData *)inUserData;

  /* myCmdData->pluginData->instance->printSomething(); */
  auto thisId = myCmdData->id;
  auto instance =
      myCmdData->pluginData->instances.at(thisId); //->printSomething();
  instance->evaluateCode(myCmdData->code);

  return true;
}

// stage3 is real time - completion msg performed if stage3 returns true
bool cmdStage3(World *world, void *inUserData) { return true; }

// stage4 is non real time - sends done if stage4 returns true
bool cmdStage4(World *world, void *inUserData) { return true; }

void cmdCleanup(World *world, void *inUserData) {
  FaustCommandData *faustCmdData = (FaustCommandData *)inUserData;

  RTFree(world, faustCmdData->code); // free the string
  RTFree(world, faustCmdData);       // free command data
  // scsynth will delete the completion message for you.
}

void receiveNewFaustCode(World *inWorld, void *inUserData,
                         struct sc_msg_iter *args, void *replyAddr) {

  // user data is the plug-in's user data.
  FaustData *thePlugInData = (FaustData *)inUserData;

  // allocate command data, free it in cmdCleanup.
  FaustCommandData *faustCmdData =
      (FaustCommandData *)RTAlloc(inWorld, sizeof(FaustCommandData));

  faustCmdData->pluginData = thePlugInData;

  // ID arguments
  faustCmdData->id = args->geti();
  faustCmdData->nodeID = args->geti();

  const char *newCode = args->gets(); // get the string argument
  if (newCode) {
    faustCmdData->code = (char *)RTAlloc(
        inWorld,
        strlen(newCode) + 1); // allocate space, free it in cmdCleanup. */
    strcpy(faustCmdData->code, newCode);

    std::cout << "Received new code: " << newCode << std::endl;
  }

  if (debug_messages) {
    Print("Global dictionary of instances of Faust UGens: ");
    for (auto &it : faustData.instances) {
      std::cout << "\t" << it.first << ": " << it.second << std::endl;
    }
  };

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

PluginLoad(FaustGenUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<FaustGen::FaustGen>(ft, "FaustGen", false);

  DefinePlugInCmd("fausteval", (PlugInCmdFunc)FaustGen::receiveNewFaustCode,
                  (void *)&FaustGen::faustData);
}
