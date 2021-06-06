#include "MergePass_RS.hlsl"

// Output from vertex shader
struct VS_OUT {
	float4 pos : SV_POSITION;
	float2 uv : TEXTCORDS;
	uint textureId : SV_InstanceID;
};

// Shader entry point
[RootSignature(MERGEPASS_RS)]
VS_OUT main(float2 pos : POSITION, uint instanceId : SV_InstanceID) {
	// Build shader output
	VS_OUT vsOutput;
	vsOutput.pos = float4(pos, 0.0f, 1.0f);
	vsOutput.uv = float2((pos.x + 1.0f) / 2.0f, (-1.0f * pos.y + 1.0f) / 2.0f);
	vsOutput.textureId = instanceId;

	// Return computet data
	return vsOutput;
}
