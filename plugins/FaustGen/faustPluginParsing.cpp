#include "faustPluginParsing.hpp"

namespace FaustGen {

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

} // namespace FaustGen
