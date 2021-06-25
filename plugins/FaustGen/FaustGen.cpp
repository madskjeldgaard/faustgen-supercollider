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
  std::string m_errorString = "did not work";

  // @fixme Not sure about this
  auto optimize = -1;

  auto argc = 0;
  const char **argv{};

  // compiling in memory (createDSPFactoryFromFile could be used alternatively)
  m_factory = createDSPFactoryFromString("faust", theCode, argc, argv, "",
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

  // hypothetical audio callback, assuming m_input/m_output are previously
  //   /* while (...) { */
  /*   m_dsp->compute(128, m_input, m_output); */
  /* } */

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
  const float *input = in(0);
  float *outbuf = out(0);

  // simple gain function
  m_dsp->compute(nSamples, (FAUSTFLOAT **)input, (FAUSTFLOAT **)outbuf);
}

} // namespace FaustGen

PluginLoad(FaustGenUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<FaustGen::FaustGen>(ft, "FaustGen", false);
}
