#ifndef RND_H
#define RND_H

const char rnd[] = "void main ()\n"
		"{\n"
		"  vec4 tmpvar_1;\n"
		"  tmpvar_1.w = 1.0;\n"
		"  tmpvar_1.xyz = sin (gl_Vertex).xyz;\n"
		"  gl_FrontColor = tmpvar_1;\n"
		"  gl_Position = ftransform();\n"
		"}\n";

#endif
