
z = play { FaustGen.ar(1)!2 * 0.5 }

f = FaustGenController.new(z,1);

// Play pink noise
(
f.eval(
"
import(\"stdfaust.lib\"); 

process = no.pink_noise;

"
);

)

// Change to a triangle
(
f.eval( "import(\"stdfaust.lib\"); process = os.triangle(180);");
)
