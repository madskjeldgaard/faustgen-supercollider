import("stdfaust.lib");

process = flanger_stereo_demo with {
	invert = checkbox("[1] Invert Flange Sum");

	flanger_stereo_demo(x,y) = x,y : *(level),*(level) : pf.flanger_stereo(dmax,curdel1,curdel2,depth,fb,invert);

	lfol = component("oscillator.lib").oscrs; // sine for left channel
	lfor = component("oscillator.lib").oscrc; // cosine for right channel
	dmax = 2048;
	dflange = 0.001 * ma.SR * hslider("[1] Flange Delay [unit:ms] [style:knob]", 10, 0, 20, 0.001);
	odflange = 0.001 * ma.SR *
	hslider("[2] Delay Offset [unit:ms] [style:knob]", 1, 0, 20, 0.001);
	freq   = hslider("[1] Speed [unit:Hz] [style:knob]", 0.5, 0, 10, 0.01);
	depth  = hslider("[2] Depth [style:knob]", 1, 0, 1, 0.001);
	fb     = hslider("[3] Feedback [style:knob]", 0, -0.999, 0.999, 0.001);
	level  = hslider("Flanger Output Level [unit:dB]", 0, -60, 10, 0.1) : ba.db2linear;
	curdel1 = odflange+dflange*(1 + lfol(freq))/2;
	curdel2 = odflange+dflange*(1 + lfor(freq))/2;
};
