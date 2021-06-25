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
struct FaustData {};
struct FaustCommandData {
  FaustData *pluginData;

  char *name;
};

FaustData faustData;

FaustGen::FaustGen() {
  // @TODO: This should be moved outside of the ugen's constructor, perhaps in a
  // seperate superCollider class the Faust code to compile as a string (could
  // be in a file too)
  std::string theCode = "import(\"stdfaust.lib\"); process = no.pink_noise;";

  auto optimize = -1;

  auto argc = 0;
  const char **argv = NULL;

  // compiling in memory (createDSPFactoryFromFile could be used alternatively)
  const auto name = "faustgen";
  m_factory = createDSPFactoryFromString(name, theCode, argc, argv, "",
                                         m_errorString, optimize);
  if (!m_factory) {
    Print("Could not create FAUST factory \n");
    std::cout << m_errorString << std::endl;

    // Setting clear function to be the calculation function when syntax or
    // other errors occur in the faust code interpretation
    mCalcFunc = make_calc_function<FaustGen, &FaustGen::clear>();

    return;
  } else {
    // Post debug info
    if (debug_messages) {
      Print("Created faust factory \n");

      const auto dspcode = m_factory->getDSPCode();

      std::cout << "faust code used: \n"
                << "\t" << dspcode << std::endl;

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

  // initializing the DSP instance with the SR
  m_dsp->init(static_cast<int>(sampleRate()));

  if (!m_dsp) {
    Print("Could not create FAUST dsp \n");
    mCalcFunc = make_calc_function<FaustGen, &FaustGen::clear>();
  } else {

    // Post debug info
    if (debug_messages) {
      Print("Created faust dsp instance \n");
      Print("name:");
      Print(name);
      Print("\nnum inputs: \n");
      Print("%d \n", m_dsp->getNumInputs());
      Print("num outputs: \n");
      Print("%d \n", m_dsp->getNumOutputs());
      Print("samplerate: \n");
      Print("%d \n", m_dsp->getSampleRate());
    }

    mCalcFunc = make_calc_function<FaustGen, &FaustGen::next>();
    next(1);
  }
}

FaustGen::~FaustGen() {
  // cleaning
  delete m_dsp;
  /* delete m_ui; */
  deleteDSPFactory(m_factory);
}

void FaustGen::clear(int nSamples) { ClearUnitOutputs(this, nSamples); }
void FaustGen::next(int nSamples) {
  // compute faust code
  m_dsp->compute(nSamples, (FAUSTFLOAT **)this->mInBuf,
                 (FAUSTFLOAT **)this->mOutBuf);
}

/**********************************************
 *
 * PLUGIN COMMUNICATION
 *
 * *******************************************/

// stage2 is non real time
bool cmdStage2(World *world, void *inUserData) {
  // user data is the command.
  FaustCommandData *myCmdData = (FaustCommandData *)inUserData;

  // just print out the values
  Print("cmdstage2\n");
  /* Print("cmdStage2 a %g  b %g  x %g  y %g  name %s\n",
   * myCmdData->myPlugin->a, */
  /*       myCmdData->myPlugin->b, myCmdData->x, myCmdData->y, myCmdData->name);
   */

  return true;
}

// stage3 is real time - completion msg performed if stage3 returns true
bool cmdStage3(World *world, void *inUserData) {
  // user data is the command.
  FaustCommandData *myCmdData = (FaustCommandData *)inUserData;
  Print("cmdstage3\n");

  // just print out the values
  /* Print("cmdStage3 a %g  b %g  x %g  y %g  name %s\n",
   * myCmdData->myPlugin->a, */
  /*       myCmdData->myPlugin->b, myCmdData->x, myCmdData->y, myCmdData->name);
   */

  // scsynth will perform completion message after this returns
  return true;
}

// stage4 is non real time - sends done if stage4 returns true
bool cmdStage4(World *world, void *inUserData) {
  // user data is the command.
  FaustCommandData *myCmdData = (FaustCommandData *)inUserData;
  Print("cmdstage4\n");

  // just print out the values
  /* Print("cmdStage4 a %g  b %g  x %g  y %g  name %s\n",
   * myCmdData->myPlugin->a, */
  /*       myCmdData->myPlugin->b, myCmdData->x, myCmdData->y, myCmdData->name);
   */

  // scsynth will send /done after this returns
  return true;
}

void cmdCleanup(World *world, void *inUserData) {
  // user data is the command.
  FaustCommandData *faustCmdData = (FaustCommandData *)inUserData;

  /* Print("cmdCleanup a %g  b %g  x %g  y %g  name %s\n",
   * faustCmdData->faust->a, */
  /*       faustCmdData->myPlugin->b, faustCmdData->x, faustCmdData->y, */
  /*       faustCmdData->name); */

  RTFree(world, faustCmdData->name); // free the string
  RTFree(world, faustCmdData);       // free command data
  // scsynth will delete the completion message for you.
}

void receiveNewFaustCode(World *inWorld, void *inUserData,
                         struct sc_msg_iter *args, void *replyAddr) {
  Print("->cmdDemoFunc %p\n", inUserData);

  // user data is the plug-in's user data.
  FaustData *thePlugInData = (FaustData *)inUserData;

  // allocate command data, free it in cmdCleanup.
  FaustCommandData *faustCmdData =
      (FaustCommandData *)RTAlloc(inWorld, sizeof(FaustCommandData));
  faustCmdData->pluginData = thePlugInData;

  // float arguments
  /* faustCmdData->x = args->getf(); */
  /* faustCmdData->y = args->getf(); */

  // how to pass a string argument:
  const char *newCode = args->gets(); // get the string argument
  if (newCode) {
    faustCmdData->name = (char *)RTAlloc(
        inWorld,
        strlen(newCode) + 1); // allocate space, free it in cmdCleanup. */
    strcpy(faustCmdData->name, newCode);

    std::cout << "Received new code: " << newCode << std::endl;
  }

  // how to pass a completion message
  int msgSize = args->getbsize();
  char *msgData = 0;
  if (msgSize) {
    // allocate space for completion message
    // scsynth will delete the completion message for you.
    msgData = (char *)RTAlloc(inWorld, msgSize);
    args->getb(msgData, msgSize); // copy completion message.
  }

  DoAsynchronousCommand(
      inWorld, replyAddr, "fausteval", (void *)faustCmdData,
      (AsyncStageFn)cmdStage2, // stage2 is non real time
      (AsyncStageFn)cmdStage3, // stage3 is real time - completion msg
                               // performed if stage3 returns true
      (AsyncStageFn)cmdStage4, // stage4 is non real time - sends done if
                               // stage4 returns true
      cmdCleanup, msgSize, msgData);
}

} // namespace FaustGen

PluginLoad(FaustGenUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<FaustGen::FaustGen>(ft, "FaustGen", false);

  DefinePlugInCmd("fausteval", (PlugInCmdFunc)FaustGen::receiveNewFaustCode,
                  (void *)&FaustGen::faustData);
}
