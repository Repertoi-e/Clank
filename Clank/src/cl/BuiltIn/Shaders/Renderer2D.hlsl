cbuffer MatrixBuffer : register(b0)
{
	matrix ViewMatrix;
	matrix ProjectionMatrix;
};

struct VShaderIn
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
	uint tid : ID;
	float4 color : COLOR;
};

struct PShaderIn
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	uint tid : ID;
	float4 color : COLOR;
};

PShaderIn VSMain(VShaderIn input)
{
	PShaderIn output;

	output.position = mul(input.position, ViewMatrix);
	output.position = mul(output.position, ProjectionMatrix);

	output.uv = input.uv;
	output.tid = input.tid;
	output.color = input.color;

	return output;
}

Texture2D textures[16] : register(t0);
SamplerState samplers[16] : register(s0);

float4 PSMain(PShaderIn input) : SV_TARGET
{
	float4 texColor = input.color;

	switch (input.tid)
	{
	case  1: texColor *= textures[0].Sample(samplers[0], input.uv); break;
	case  2: texColor *= textures[1].Sample(samplers[1], input.uv); break;
	case  3: texColor *= textures[2].Sample(samplers[2], input.uv); break;
	case  4: texColor *= textures[3].Sample(samplers[3], input.uv); break;
	case  5: texColor *= textures[4].Sample(samplers[4], input.uv); break;
	case  6: texColor *= textures[5].Sample(samplers[5], input.uv); break;
	case  7: texColor *= textures[6].Sample(samplers[6], input.uv); break;
	case  8: texColor *= textures[7].Sample(samplers[7], input.uv); break;
	case  9: texColor *= textures[8].Sample(samplers[8], input.uv); break;
	case 10: texColor *= textures[9].Sample(samplers[9], input.uv); break;
	case 11: texColor *= textures[10].Sample(samplers[10], input.uv); break;
	case 12: texColor *= textures[11].Sample(samplers[11], input.uv); break;
	case 13: texColor *= textures[12].Sample(samplers[12], input.uv); break;
	case 14: texColor *= textures[13].Sample(samplers[13], input.uv); break;
	case 15: texColor *= textures[14].Sample(samplers[14], input.uv); break;
	}

	// texColor.w = 0.5f;

	return texColor;
}