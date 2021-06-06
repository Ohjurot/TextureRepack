#include "MergePass_RS.hlsl"

struct PS_IN{
	float4 pos : SV_POSITION;
	float2 uv : TEXTCORDS;
	uint textureId : SV_InstanceID;
};

// Root Signature types
Texture2D<float1> maskTexture[] : register(t0, space0);
Texture2D<float4> drawTexture[] : register(t0, space1);
sampler textureSampler : register(s0);

// Shader entry point
[RootSignature(MERGEPASS_RS)]
float4 main(PS_IN input) : SV_TARGET{
	float mask = maskTexture[input.textureId].Sample(textureSampler, input.uv).r;
	float3 color = drawTexture[input.textureId].Sample(textureSampler, input.uv).rgb;

	return float4(mask * color, mask);
}
