FaustGen : UGen {
	*ar { |id, in1|
		^this.multiNew('audio', id, in1);
	}

	// *kr { |id|
	// 	^this.multiNew('control', id);
	// }

	checkInputs {
		^this.checkValidInputs;
	}
}
