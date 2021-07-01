FaustGen : UGen {
	*ar { |id|
		^this.multiNew('audio', id);
	}

	// *kr { |id|
	// 	^this.multiNew('control', id);
	// }

	checkInputs {
		^this.checkValidInputs;
	}
}
