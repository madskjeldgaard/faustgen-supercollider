// PluginFaustGen.hpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#pragma once

#include "SC_PlugIn.hpp"
#include "faust/dsp/libfaust.h"
#include "faust/dsp/llvm-dsp.h"

namespace FaustGen {

class FaustGen : public SCUnit {
public:
  FaustGen();

  // Destructor
  ~FaustGen();

private:
  // Calc function
  void next(int nSamples);
  dsp *m_dsp;
  llvm_dsp_factory *m_factory;

  // Member variables
};

} // namespace FaustGen
