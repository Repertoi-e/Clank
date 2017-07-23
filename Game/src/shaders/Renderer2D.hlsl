cbuffer MatrixBuffer
{
    matrix ModelMatrix;
    matrix ViewMatrix;
    matrix ProjectionMatrix;
};

struct VShaderIn
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PShaderIn
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PShaderIn VSMain(VShaderIn input)
{
    PShaderIn output;

    output.position = float4(input.position, 1.0);
    output.position = mul(output.position, ModelMatrix);
    output.position = mul(output.position, ViewMatrix);
    output.position = mul(output.position, ProjectionMatrix);

    output.color = input.color;

    return output;
}

float4 PSMain(PShaderIn input) : SV_TARGET
{
    return input.color;
}