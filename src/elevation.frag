varying float intensity;

void main()
{
	vec4 color;
	color = vec4(intensity,intensity,intensity,1.0);
	gl_FragColor = color;
}