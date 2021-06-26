// PluginFaustGen.hpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#pragma once

#include "SC_PlugIn.hpp"
#include "faust/dsp/libfaust.h"
#include "faust/dsp/llvm-dsp.h"

/* #define MAX_FAUST_INPUTS 1 */
/* #define MAX_FAUST_OUTPUTS 1 */
namespace FaustGen {

class FaustGen : public SCUnit {
public:
  FaustGen();

  // Destructor
  ~FaustGen();

  /* void evalCmd(FaustGen *unit, sc_msg_iter *args); */
  void printDSPInfo();   
  void evaluateCode(char *code);
  int id;

  int mNumAudioInputs;

private:

  bool parse(char *theCode);
  void next(int nSamples);
  void clear(int nSamples);
  dsp *m_dsp;
  llvm_dsp_factory *m_factory;

  std::string m_errorString;

  // only includes non audio params
  enum InputName { Id, NumParameters };


  /* FAUSTFLOAT **faustinputs[MAX_FAUST_INPUTS]; */
  /* FAUSTFLOAT **faustoutputs[MAX_FAUST_OUTPUTS]; */
};

} // namespace FaustGen
