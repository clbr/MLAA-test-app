varying vec4 offset[2];

uniform sampler2D blendMap;
uniform sampler2D colorMap;

void main() {
	// Fetch the blending weights for current pixel:
	vec4 topLeft = texture2D(blendMap, gl_TexCoord[0].xy);
	float bottom = texture2D(blendMap, offset[1].zw).g;
	float right = texture2D(blendMap, offset[1].xy).a;
	vec4 a = vec4(topLeft.r, bottom, topLeft.b, right);

	// Up to 4 lines can be crossing a pixel (one in each edge). So, we perform
	// a weighted average, where the weight of each line is 'a' cubed, which
	// favors blending and works well in practice.
	vec4 w = a * a * a;

	// There is some blending weight with a value greater than 0.0?
	float sum = dot(w, vec4(1.0));
	if (sum < 1e-5)
		discard;

	vec4 color = vec4(0.0);

	// Add the contributions of the possible 4 lines that can cross this pixel:
	vec4 C = texture2D(colorMap, gl_TexCoord[0].xy);
	vec4 Cleft = texture2D(colorMap, offset[0].xy);
	vec4 Ctop = texture2D(colorMap, offset[0].zw);
	vec4 Cright = texture2D(colorMap, offset[1].xy);
	vec4 Cbottom = texture2D(colorMap, offset[1].zw);
	color = mix(C, Ctop, a.r) * w.r + color;
	color = mix(C, Cbottom, a.g) * w.g + color;
	color = mix(C, Cleft, a.b) * w.b + color;
	color = mix(C, Cright, a.a) * w.a + color;

	// Normalize the resulting color and we are finished!
	gl_FragColor = color / sum;
}
