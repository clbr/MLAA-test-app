varying vec4 offset[2];

void main() {
	gl_Position = gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;

	offset[0] = gl_MultiTexCoord0.xyxy + PIXEL_SIZE.xyxy * vec4(-1.0, 0.0, 0.0, -1.0);
	offset[1] = gl_MultiTexCoord0.xyxy + PIXEL_SIZE.xyxy * vec4( 1.0, 0.0, 0.0,  1.0);
}
