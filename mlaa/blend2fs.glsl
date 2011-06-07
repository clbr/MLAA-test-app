#extension GL_ARB_shader_texture_lod: enable

uniform sampler2D edgesMapL;

/**
 * This one just returns the first level of a mip map chain, which allow us to
 * avoid the nasty ddx/ddy warnings, even improving the performance a little
 * bit.
 */
vec4 tex2Doffset(sampler2D map, vec2 texcoord, vec2 offset) {
	return texture2DLod(map, texcoord + PIXEL_SIZE * offset, 0.0);
}

float SearchXLeft(vec2 texcoord) {
	// We compare with 0.9 to prevent bilinear access precision problems.
	float i;
	float e = 0.0;
	for (i = -1.5; i > -2.0 * MAX_SEARCH_STEPS; i -= 2.0) {
		e = tex2Doffset(edgesMapL, texcoord, vec2(i, 0.0)).g;
		if (e < 0.9) break;
	}
	return max(i + 1.5 - 2.0 * e, -2.0 * MAX_SEARCH_STEPS);
}

float SearchXRight(vec2 texcoord) {
	float i;
	float e = 0.0;
	for (i = 1.5; i < 2.0 * MAX_SEARCH_STEPS; i += 2.0) {
		e = tex2Doffset(edgesMapL, texcoord, vec2(i, 0.0)).g;
		if (e < 0.9) break;
	}
	return min(i - 1.5 + 2.0 * e, 2.0 * MAX_SEARCH_STEPS);
}

float SearchYDown(vec2 texcoord) {
	float i;
	float e = 0.0;
	for (i = -1.5; i > -2.0 * MAX_SEARCH_STEPS; i -= 2.0) {
		e = tex2Doffset(edgesMapL, texcoord, vec2(i, 0.0).yx).r;
		if (e < 0.9) break;
	}
	return max(i + 1.5 - 2.0 * e, -2.0 * MAX_SEARCH_STEPS);
}

float SearchYUp(vec2 texcoord) {
	float i;
	float e = 0.0;
	for (i = 1.5; i < 2.0 * MAX_SEARCH_STEPS; i += 2.0) {
		e = tex2Doffset(edgesMapL, texcoord, vec2(i, 0.0).yx).r;
		if (e < 0.9) break;
	}
	return min(i - 1.5 + 2.0 * e, 2.0 * MAX_SEARCH_STEPS);
}

