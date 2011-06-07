#ifndef NEIGH3FS_H
#define NEIGH3FS_H

const char neigh3fs[] = "varying vec4 offset[2];\n"
		"\n"
		"uniform sampler2D blendMap;\n"
		"uniform sampler2D colorMap;\n"
		"\n"
		"void main() {\n"
		"	// Fetch the blending weights for current pixel:\n"
		"	vec4 topLeft = texture2D(blendMap, gl_TexCoord[0].xy);\n"
		"	float bottom = texture2D(blendMap, offset[1].zw).g;\n"
		"	float right = texture2D(blendMap, offset[1].xy).a;\n"
		"	vec4 a = vec4(topLeft.r, bottom, topLeft.b, right);\n"
		"\n"
		"	// Up to 4 lines can be crossing a pixel (one in each edge). So, we perform\n"
		"	// a weighted average, where the weight of each line is 'a' cubed, which\n"
		"	// favors blending and works well in practice.\n"
		"	vec4 w = a * a * a;\n"
		"\n"
		"	// There is some blending weight with a value greater than 0.0?\n"
		"	float sum = dot(w, vec4(1.0));\n"
		"	if (sum < 1e-5)\n"
		"		discard;\n"
		"\n"
		"	vec4 color = vec4(0.0);\n"
		"\n"
		"	// Add the contributions of the possible 4 lines that can cross this pixel:\n"
		"	vec4 C = texture2D(colorMap, gl_TexCoord[0].xy);\n"
		"	vec4 Cleft = texture2D(colorMap, offset[0].xy);\n"
		"	vec4 Ctop = texture2D(colorMap, offset[0].zw);\n"
		"	vec4 Cright = texture2D(colorMap, offset[1].xy);\n"
		"	vec4 Cbottom = texture2D(colorMap, offset[1].zw);\n"
		"	color = mix(C, Ctop, a.r) * w.r + color;\n"
		"	color = mix(C, Cbottom, a.g) * w.g + color;\n"
		"	color = mix(C, Cleft, a.b) * w.b + color;\n"
		"	color = mix(C, Cright, a.a) * w.a + color;\n"
		"\n"
		"	// Normalize the resulting color and we are finished!\n"
		"	gl_FragColor = color / sum;\n"
		"}\n";

#endif
