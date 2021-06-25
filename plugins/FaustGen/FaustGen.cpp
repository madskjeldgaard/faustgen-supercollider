// PluginFaustGen.cpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#include "SC_PlugIn.hpp"
#include "FaustGen.hpp"

static InterfaceTable* ft;

namespace FaustGen {

FaustGen::FaustGen() {
    mCalcFunc = make_calc_function<FaustGen, &FaustGen::next>();
    next(1);
}

void FaustGen::next(int nSamples) {
    const float* input = in(0);
    const float* gain = in(1);
    float* outbuf = out(0);

    // simple gain function
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = input[i] * gain[i];
    }
}

} // namespace FaustGen

PluginLoad(FaustGenUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<FaustGen::FaustGen>(ft, "FaustGen", false);
}
