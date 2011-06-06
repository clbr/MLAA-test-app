varying vec4 offset[2];

uniform sampler2D depthMap;
const float threshold = 0.1f;

void main() {

	float D = texture2D(depthMap, gl_TexCoord[0].xy).r;
	float Dleft = texture2D(depthMap, offset[0].xy).r;
	float Dtop = texture2D(depthMap, offset[0].zw).r;
	float Dright = texture2D(depthMap, offset[1].xy).r;
	float Dbottom = texture2D(depthMap, offset[1].zw).r;

	vec4 delta = abs(vec4(D) - vec4(Dleft, Dtop, Dright, Dbottom));
	vec4 edges = step(vec4(threshold)/10.0, delta); // Needs division by 10 to be similar to the color one

	if (dot(edges, vec4(1.0)) == 0.0)
		discard;

	gl_FragColor = edges;
}
