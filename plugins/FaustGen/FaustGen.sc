FaustGen : UGen {
	*ar { |id|
		^this.multiNew('audio', id);
	}
	checkInputs {
		^this.checkValidInputs;
	}
}
