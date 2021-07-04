FaustGen : MultiOutUGen {
  classvar numChannelsOut;
	*ar { |id, numChansOut=2 ... audioInputs|
      var maxFaustOutputs = 64;
      var numChansIn = audioInputs.flatten.size;
      numChannelsOut = numChansOut;

      if(numChansIn > maxFaustOutputs, { 
        "FaustGen: Too many inputs.\n The maximum amount of inputs allowed: %.\n You\'ve input % channels.\n".format(maxFaustOutputs, numChansIn).error;
        ^nil
      });

      ^this.multiNew('audio', id, *audioInputs);
	}

    init { arg ... theInputs;
      inputs = theInputs;

      ^this.initOutputs(numChannelsOut, rate)
    }

	// *kr { |id|
	// 	^this.multiNew('control', id);
	// }

	checkInputs {
      // @TODO check number of inputs 
		^this.checkValidInputs;
	}
}
