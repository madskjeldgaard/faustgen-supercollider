// PluginFaustGen.hpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#pragma once

#include "SC_PlugIn.hpp"
#include "faust/dsp/libfaust.h"
#include "faust/dsp/llvm-dsp.h"

#define MAX_FAUST_INPUTS 64
#define MAX_FAUST_OUTPUTS 64
namespace FaustGen {

class FaustGen : public SCUnit {
public:
  FaustGen();

  // Destructor
  ~FaustGen();

  int id;

  int mNumAudioInputs;

  void setNewDSP(dsp* newDsp);

private:
  void next(int nSamples);
  void clear(int nSamples);
  dsp *m_dsp;
  bool m_hasDSP{false};

  std::string m_errorString;

  // only includes non audio params
  enum Inputs { Id, NumPreAudioParameters };


  FAUSTFLOAT **faustinputs[MAX_FAUST_INPUTS];
  /* FAUSTFLOAT **faustoutputs[MAX_FAUST_OUTPUTS]; */
};

} // namespace FaustGen
