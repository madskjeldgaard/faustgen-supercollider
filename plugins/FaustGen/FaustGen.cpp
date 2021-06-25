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

void FaustGen::next(int nSamples) {
  /* const FAUSTFLOAT *input = in(0); */

  // Copy inputs to faust buffer
  /* for (size_t i = 0; i < nSamples; i++) { */
  /*   faustinputs[0][i] = input[i]; */
  /* } */

  // Flush faust input buffer
  /* for (size_t in_num; in_num < m_numinputs; in_num++) { */
  /*   for (size_t i = 0; i < nSamples; i++) { */
  /*     memset(faustinputs[in_num], 0.0, nSamples * sizeof(FAUSTFLOAT)); */
  /*   } */
  /* } */

  // Flush faust output buffer
  // @FIXME probably not necessary
  for (size_t out_num; out_num < m_numoutputs; out_num++) {
      memset(faustoutputs[out_num], 0.0, nSamples * sizeof(FAUSTFLOAT));
  }

  // compute faust code
  m_dsp->compute(nSamples, (FAUSTFLOAT **)faustinputs,
                 (FAUSTFLOAT **)faustoutputs);

  // Copy to output
  for (size_t out_num; out_num < m_numoutputs; out_num++) {
    for (size_t i = 0; i < nSamples; i++) {
      /* out(out_num)[i] = *faustoutputs[out_num][i]; */
      out(out_num)[i] = 0;
    }
  }
}
} // namespace FaustGen

PluginLoad(FaustGenUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<FaustGen::FaustGen>(ft, "FaustGen", false);
}
