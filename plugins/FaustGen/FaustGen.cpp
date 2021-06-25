// PluginFaustGen.cpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#include "FaustGen.hpp"
#include "SC_PlugIn.hpp"
#include <string>

static InterfaceTable *ft;

namespace FaustGen {

FaustGen::FaustGen() {
  // the Faust code to compile as a string (could be in a file too)
  std::string theCode = "import(\"stdfaust.lib\"); process = no.noise;";
  std::string m_errorString;

  // @fixme Not sure about this
  auto optimize = -4;

  auto argc = 0;
  const char **argv;

  // compiling in memory (createDSPFactoryFromFile could be used alternatively)
  const auto name = "faustgen";
  m_factory = createDSPFactoryFromString(name, theCode, argc, argv, "",
                                         m_errorString, optimize);
  if (m_factory == nullptr) {
    auto errmsg = "Could not create FAUST dsp factory \n";
    Print(errmsg);
    return;
  }

  // creating the DSP instance for interfacing
  m_dsp = m_factory->createDSPInstance();

  // creating a generic UI to interact with the DSP
  /* my_ui *m_ui = new MyUI(); */

  // linking the interface to the DSP instance
  /* m_dsp->buildUserInterface(m_ui); */

  // initializing the DSP instance with the SR
  m_dsp->init(static_cast<int>(sampleRate()));

  // Post debug info
  Print("Created faust dsp instance \n");
  Print("name:");
  Print(name);
  Print("\nnum inputs: \n");
  Print("%d \n", m_dsp->getNumInputs());
  Print("num outputs: \n");
  Print("%d \n", m_dsp->getNumOutputs());
  Print("samplerate: \n");
  Print("%d \n", m_dsp->getSampleRate());

  mCalcFunc = make_calc_function<FaustGen, &FaustGen::next>();
  next(1);
}

FaustGen::~FaustGen() {
  // cleaning
  delete m_dsp;
  /* delete m_ui; */
  deleteDSPFactory(m_factory);
}

void FaustGen::next(int nSamples) {
  const FAUSTFLOAT *input = in(0);
  float *outbuf = out(0);

  FAUSTFLOAT **faustinputs{};
  FAUSTFLOAT **faustoutputs{};

  // Copy inputs to faust buffer
  /* for (size_t i = 0; i < nSamples; i++) { */
  /*   faustinputs[0][i] = input[i]; */
  /* } */

  // compute faust code
  m_dsp->compute(nSamples, faustinputs, faustoutputs);

  // Copy to output
  for (size_t i = 0; i < nSamples; i++) {
    Print("hello\n");
    outbuf[i] = faustoutputs[0][i];
  }
}
} // namespace FaustGen

PluginLoad(FaustGenUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<FaustGen::FaustGen>(ft, "FaustGen", false);
}
