// PluginFaustGen.cpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#include "FaustGen.hpp"
#include "SC_PlugIn.hpp"
#include <iostream>
#include <string>

static InterfaceTable *ft;

namespace FaustGen {
constexpr bool debug_messages = true;

FaustGen::FaustGen() {
  // the Faust code to compile as a string (could be in a file too)
  std::string theCode = "import(\"stdfaust.lib\"); process = no.noise;";
  std::string m_errorString;

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
} // namespace FaustGen

PluginLoad(FaustGenUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<FaustGen::FaustGen>(ft, "FaustGen", false);
}
