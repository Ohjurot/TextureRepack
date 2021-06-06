#include "MaskPass_RS.hlsl"

// Shader entry point
[RootSignature(MASKPASS_RS)]
float4 ps_main() : SV_TARGET {
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
