#ifndef OFFSETVS_H
#define OFFSETVS_H

const char offsetvs[] = "varying vec4 offset[2];\n"
		"void main() {\n"
		"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"	gl_TexCoord[0] = gl_MultiTexCoord0;\n"
		"	gl_TexCoord[0].y = 1.0 - gl_MultiTexCoord0.y;\n"
		"\n"
		"	offset[0] = gl_TexCoord[0].xyxy + PIXEL_SIZE.xyxy * vec4(-1.0, 0.0, 0.0,  1.0);\n"
		"	offset[1] = gl_TexCoord[0].xyxy + PIXEL_SIZE.xyxy * vec4( 1.0, 0.0, 0.0, -1.0);\n"
		"}\n";

#endif
