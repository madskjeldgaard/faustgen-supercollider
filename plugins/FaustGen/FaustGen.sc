FaustGen : UGen {
	*ar { |id, audioInput|
		^this.multiNew('audio', id, audioInput);
	}

	// *kr { |id|
	// 	^this.multiNew('control', id);
	// }

	checkInputs {
		^this.checkValidInputs;
	}
}
