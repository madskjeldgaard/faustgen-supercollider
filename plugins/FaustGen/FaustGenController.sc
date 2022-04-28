FaustGenController {
	var server, id, nodeId;
    *new {|synth,id|
        ^super.newCopyArgs(synth.server, id, synth.nodeID);
    }

    eval{ arg faustCode;
        server.sendMsg('\cmd', '\fausteval', id, nodeId, faustCode)
    }

    evalFile{ arg faustfile;
        var faustCode = File.readAllString(faustfile);
        this.eval(faustCode)
    }

	<< { arg faustCode;
		this.eval(faustCode)
	}

    *exampleFiles{
        var files = PathName(
            Main.packages.asDict.at('FaustGen') +/+ "faust-examples"
        )
        .files
        .select({|faustfile|
            faustfile.extension == "dsp"
        });
        var dict = IdentityDictionary.new;

        files.do{|file| dict.put(file.fileName.asSymbol, file.fullPath)}

        ^dict
    }
}
