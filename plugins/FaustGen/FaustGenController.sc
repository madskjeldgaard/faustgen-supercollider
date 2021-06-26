FaustGenController {
	var server, id, nodeId;
    *new {|synth,id| 
        ^super.newCopyArgs(synth.server, id, synth.nodeID);
    }

    eval{ arg faustCode;
        server.sendMsg('\cmd', '\fausteval', id, nodeId, faustCode)
    }

	<< { arg faustCode; 
		this.eval(faustCode)
	}

}


Post
