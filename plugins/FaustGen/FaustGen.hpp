// PluginFaustGen.hpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#pragma once

#include "SC_PlugIn.hpp"
#include "faust/dsp/libfaust.h"
#include "faust/dsp/llvm-dsp.h"

#define MAX_FAUST_INPUTS 1
#define MAX_FAUST_OUTPUTS 1
namespace FaustGen {

class FaustGen : public SCUnit {
public:
  FaustGen();

  // Destructor
  ~FaustGen();

private:
  int m_numoutputs {1}, m_numinputs{0};

  void next(int nSamples);
  dsp *m_dsp;
  llvm_dsp_factory *m_factory;

  FAUSTFLOAT **faustinputs[MAX_FAUST_INPUTS];
  FAUSTFLOAT **faustoutputs[MAX_FAUST_OUTPUTS];
};

} // namespace FaustGen
