FaustGen : UGen {
	*ar { |input|
		/* TODO */
		^this.multiNew('audio', input);
	}
	checkInputs {
		/* TODO */
		^this.checkValidInputs;
	}
}
