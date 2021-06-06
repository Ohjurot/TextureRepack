#include "MaskPass_RS.hlsl"

// Defines a vertex
struct VERTEX {
	float2 uv : TEXCORDS;
};

// Shader entry point
[RootSignature(MASKPASS_RS)]
float4 vs_main(VERTEX vsIn) : SV_POSITION{
	// Output position
	return float4(-1.0f + vsIn.uv.x * 2.0f, 1.0f - vsIn.uv.y * 2.0f, 0.0f, 1.0f);
}
