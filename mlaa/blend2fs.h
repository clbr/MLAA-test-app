#ifndef BLEND2FS_H
#define BLEND2FS_H

const char blend2fs[] = "#extension GL_ARB_shader_texture_lod: enable\n"
		"\n"
		"uniform sampler2D edgesMapL;\n"
		"uniform sampler2D edgesMap;\n"
		"uniform sampler2D areaMap;\n"
		"\n"
		"/**\n"
		" * This one just returns the first level of a mip map chain, which allow us to\n"
		" * avoid the nasty ddx/ddy warnings, even improving the performance a little\n"
		" * bit.\n"
		" */\n"
		"vec4 tex2Doffset(sampler2D map, vec2 texcoord, vec2 offset) {\n"
		"	return texture2DLod(map, texcoord + PIXEL_SIZE * offset, 0.0);\n"
		"}\n"
		"\n"
		"float SearchXLeft(vec2 texcoord) {\n"
		"	// We compare with 0.9 to prevent bilinear access precision problems.\n"
		"	float i;\n"
		"	float e = 0.0;\n"
		"	for (i = -1.5; i > -2.0 * MAX_SEARCH_STEPS; i -= 2.0) {\n"
		"		e = tex2Doffset(edgesMapL, texcoord, vec2(i, 0.0)).g;\n"
		"		if (e < 0.9) break;\n"
		"	}\n"
		"	return max(i + 1.5 - 2.0 * e, -2.0 * MAX_SEARCH_STEPS);\n"
		"}\n"
		"\n"
		"float SearchXRight(vec2 texcoord) {\n"
		"	float i;\n"
		"	float e = 0.0;\n"
		"	for (i = 1.5; i < 2.0 * MAX_SEARCH_STEPS; i += 2.0) {\n"
		"		e = tex2Doffset(edgesMapL, texcoord, vec2(i, 0.0)).g;\n"
		"		if (e < 0.9) break;\n"
		"	}\n"
		"	return min(i - 1.5 + 2.0 * e, 2.0 * MAX_SEARCH_STEPS);\n"
		"}\n"
		"\n"
		"float SearchYDown(vec2 texcoord) {\n"
		"	float i;\n"
		"	float e = 0.0;\n"
		"	for (i = -1.5; i > -2.0 * MAX_SEARCH_STEPS; i -= 2.0) {\n"
		"		e = tex2Doffset(edgesMapL, texcoord, vec2(i, 0.0).yx).r;\n"
		"		if (e < 0.9) break;\n"
		"	}\n"
		"	return max(i + 1.5 - 2.0 * e, -2.0 * MAX_SEARCH_STEPS);\n"
		"}\n"
		"\n"
		"float SearchYUp(vec2 texcoord) {\n"
		"	float i;\n"
		"	float e = 0.0;\n"
		"	for (i = 1.5; i < 2.0 * MAX_SEARCH_STEPS; i += 2.0) {\n"
		"		e = tex2Doffset(edgesMapL, texcoord, vec2(i, 0.0).yx).r;\n"
		"		if (e < 0.9) break;\n"
		"	}\n"
		"	return min(i - 1.5 + 2.0 * e, 2.0 * MAX_SEARCH_STEPS);\n"
		"}\n"
		"\n"
		"\n"
		"vec2 round(vec2 invec) {\n"
		"	vec2 outvec;\n"
		"\n"
		"	if (fract(invec.x) >= 0.5) outvec.x = ceil(invec.x);\n"
		"	else outvec.x = floor(invec.x);\n"
		"\n"
		"	if (fract(invec.y) >= 0.5) outvec.y = ceil(invec.y);\n"
		"	else outvec.y = floor(invec.y);\n"
		"\n"
		"	return outvec;\n"
		"}\n"
		"\n"
		"vec2 Area(vec2 distance, float e1, float e2) {\n"
		"	// * By dividing by areaSize - 1.0 below we are implicitely offsetting to\n"
		"	//   always fall inside of a pixel\n"
		"	// * Rounding prevents bilinear access precision problems\n"
		"	float areaSize = MAX_DISTANCE * 5.0;\n"
		"	vec2 pixcoord = MAX_DISTANCE * round(4.0 * vec2(e1, e2)) + distance;\n"
		"	vec2 texcoord = pixcoord / (areaSize - 1.0);\n"
		"	return texture2DLod(areaMap, texcoord, 0.0).ra;\n"
		"}\n"
		"\n"
		"void main() {\n"
		"	vec4 areas = vec4(0.0);\n"
		"\n"
		"	vec2 e = texture2D(edgesMap, gl_TexCoord[0].xy).rg;\n"
		"\n"
		"	if (e.g != 0.0) { // Edge at north\n"
		"\n"
		"		// Search distances to the left and to the right:\n"
		"		vec2 d = vec2(SearchXLeft(gl_TexCoord[0].xy), SearchXRight(gl_TexCoord[0].xy));\n"
		"\n"
		"		// Now fetch the crossing edges. Instead of sampling between edgels, we\n"
		"		// sample at -0.25, to be able to discern what value has each edgel:\n"
		"		vec4 coords = vec4(d.x, -0.25, d.y + 1.0, -0.25) * PIXEL_SIZE.xyxy + gl_TexCoord[0].xyxy;\n"
		"		float e1 = texture2DLod(edgesMapL, coords.xy, 0.0).r;\n"
		"		float e2 = texture2DLod(edgesMapL, coords.zw, 0.0).r;\n"
		"\n"
		"		// Ok, we know how this pattern looks like, now it is time for getting\n"
		"		// the actual area:\n"
		"		areas.rg = Area(abs(d), e1, e2);\n"
		"	}\n"
		"\n"
		"	if (e.r != 0.0) { // Edge at west\n"
		"\n"
		"		// Search distances to the top and to the bottom:\n"
		"		vec2 d = vec2(SearchYUp(gl_TexCoord[0].xy), SearchYDown(gl_TexCoord[0].xy));\n"
		"\n"
		"		// Now fetch the crossing edges (yet again):\n"
		"		vec4 coords = vec4(-0.25, d.x, -0.25, d.y + 1.0) * PIXEL_SIZE.xyxy + gl_TexCoord[0].xyxy;\n"
		"		float e1 = texture2DLod(edgesMapL, coords.xy, 0.0).g;\n"
		"		float e2 = texture2DLod(edgesMapL, coords.zw, 0.0).g;\n"
		"\n"
		"		// Get the area for this direction:\n"
		"		areas.ba = Area(abs(d), e1, e2);\n"
		"	}\n"
		"\n"
		"	gl_FragColor = areas;\n"
		"}\n";

#endif
