// PluginFaustGen.cpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#include "FaustGen.hpp"
#include "SC_PlugIn.hpp"
#include "faustPluginCommand.hpp"
#include <iostream>
#include <string>

InterfaceTable *ft;

namespace FaustGen {


constexpr bool debug_messages = true;

extern FaustData faustData;

/**********************************************
 *
 * UGen guts
 *
 * *******************************************/
void FaustGen::setNewDSP(dsp *newDsp) {

  if (!newDsp) {
    Print("Could not create FAUST dsp \n");
    m_hasDSP = false;

    // Check number of outputs to avoid crashing
  } else if (newDsp->getNumOutputs() > this->mNumOutputs) {

    std::cout << "Error: Number of faust code outputs does not correspond to "
                 "UGen's number of outputs \n"
              << "Num UGEN outputs: " << this->mNumOutputs << std::endl;
    m_hasDSP = false;
    printDSPInfo(newDsp);

    // Check number of inputs to avoid crashing
  } else if (newDsp->getNumInputs() > mNumAudioInputs) {

    std::cout << "Error: Number of faust code inputs does not correspond to "
                 "UGen's number of inputs \n"
              << "Num UGEN inputs: "
              << this->mNumInputs - Inputs::NumPreAudioParameters << std::endl;
    m_hasDSP = false;
    printDSPInfo(newDsp);
  } else {

    // If succesful, set the DSP to the new dsp
    /* // @TODO should this be clone + RTAlloc? */
    /* m_dsp = (dsp *)RTAlloc(mWorld, sizeof(dsp)); */
    m_dsp = newDsp;
    m_hasDSP = true;

    // Post debug info
    if (debug_messages) {
      printDSPInfo(newDsp);
    }
  };
}

FaustGen::FaustGen() {
  mNumAudioInputs = mNumInputs - Inputs::NumPreAudioParameters;

  // Insert instance into global data space
  id = static_cast<int>(in0(Inputs::Id));
  std::pair<int, FaustGen *> instance(id, this);
  faustData.instances.insert(instance);
  mCalcFunc = make_calc_function<FaustGen, &FaustGen::next>();
}

FaustGen::~FaustGen() {

  // @TODO realtime safe
  if (m_hasDSP)
    /* RTFree(mWorld, m_dsp); */
    delete m_dsp;
  m_hasDSP = false;

  // @TODO Realtime safe!
  faustData.instances.erase(id);

  /* for (size_t i = 0; i < MAX_FAUST_OUTPUTS; i++){ */
  /* RTFree(mWorld, faustoutputs[i]); */
  /* }; */
}

void FaustGen::clear(int nSamples) { ClearUnitOutputs(this, nSamples); }
void FaustGen::next(int nSamples) {
	
  // compute faust code
  if (m_hasDSP) {
    m_dsp->compute(nSamples, (FAUSTFLOAT **)mInBuf + Inputs::NumPreAudioParameters,
                   (FAUSTFLOAT **)mOutBuf);
  } else {
    ClearUnitOutputs(this, nSamples);
  };
}

} // namespace FaustGen

PluginLoad(FaustGenUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<FaustGen::FaustGen>(ft, "FaustGen", true);

  DefinePlugInCmd("fausteval", (PlugInCmdFunc)FaustGen::receiveNewFaustCode,
                  (void *)&FaustGen::faustData);
}
