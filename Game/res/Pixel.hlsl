struct PShaderIn
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 main(PShaderIn input) : SV_TARGET
{
	return input.color;
}