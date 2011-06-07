#ifndef COLOR1FS_H
#define COLOR1FS_H

const char color1fs[] = "varying vec4 offset[2];\n"
		"\n"
		"uniform sampler2D colorMapG;\n"
		"const float threshold = 0.1f;\n"
		"\n"
		"void main() {\n"
		"	vec3 weights = vec3(0.2126,0.7152, 0.0722); // ITU-R BT. 709\n"
		"\n"
		"	/**\n"
		"	 * Luma calculation requires gamma-corrected colors:\n"
		"	 */\n"
		"	float L = dot(texture2D(colorMapG, gl_TexCoord[0].xy).rgb, weights);\n"
		"	float Lleft = dot(texture2D(colorMapG, offset[0].xy).rgb, weights);\n"
		"	float Ltop = dot(texture2D(colorMapG, offset[0].zw).rgb, weights);\n"
		"	float Lright = dot(texture2D(colorMapG, offset[1].xy).rgb, weights);\n"
		"	float Lbottom = dot(texture2D(colorMapG, offset[1].zw).rgb, weights);\n"
		"\n"
		"	vec4 delta = abs(vec4(L) - vec4(Lleft, Ltop, Lright, Lbottom));\n"
		"	vec4 edges = step(vec4(threshold), delta);\n"
		"\n"
		"	if (dot(edges, vec4(1.0)) == 0.0)\n"
		"		discard;\n"
		"\n"
		"	gl_FragColor = edges;\n"
		"}\n";

#endif
