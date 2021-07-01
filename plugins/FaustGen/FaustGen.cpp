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
  } else if (newDsp->getNumOutputs() != this->mNumOutputs) {

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
              << this->mNumInputs - InputName::NumParameters << std::endl;
    m_hasDSP = false;
    printDSPInfo(newDsp);
  } else {

    // If succesful, set the DSP to the new dsp
    m_dsp = newDsp;
    m_hasDSP = true;

    // Post debug info
    if (debug_messages) {
      printDSPInfo(newDsp);
    }
  };
}

FaustGen::FaustGen() {
  /* mNumAudioInputs = this->mNumInputs - InputName::NumParameters; */

  // Insert instance into global data space
  id = static_cast<int>(in0(InputName::Id));
  std::pair<int, FaustGen *> instance(id, this);
  faustData.instances.insert(instance);
  mCalcFunc = make_calc_function<FaustGen, &FaustGen::next>();
}

FaustGen::~FaustGen() {
  // cleaning
  delete m_dsp;
  /* delete m_ui; */
  /* deleteDSPFactory(m_factory); */

  faustData.instances.erase(id);
}

void FaustGen::clear(int nSamples) { ClearUnitOutputs(this, nSamples); }
void FaustGen::next(int nSamples) {

  FAUSTFLOAT **faustInputs;

  /* Remove inputs used by the UGen at init */
  /* for (size_t in_num; in_num < mNumAudioInputs; in_num++) { */
  /*   const auto offset = mNumAudioInputs; */
  /*   faustInputs[in_num] = this->mInBuf[in_num + offset]; */
  /* } */
  faustInputs = this->mInBuf;

  // compute faust code
  if (m_hasDSP) {
    m_dsp->compute(nSamples, faustInputs, (FAUSTFLOAT **)this->mOutBuf);
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
