// PluginFaustGen.hpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#pragma once

#include "SC_PlugIn.hpp"

namespace FaustGen {

class FaustGen : public SCUnit {
public:
    FaustGen();

    // Destructor
    // ~FaustGen();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace FaustGen
