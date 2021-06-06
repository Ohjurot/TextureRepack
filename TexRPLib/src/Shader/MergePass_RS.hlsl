/*
 * RootSignature:
 * 0 - Table of n SRVs	(Masks)					space0	t0 ... 
 * 1 - Table of n SRVs	(Textures)				space1	t0 ...
 * 2 - Static Sampler	(Texture filtering)		space0	s0
*/

#define MERGEPASS_RS ""\
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT ),"\
"DescriptorTable("\
"   SRV(t0, space=0, numDescriptors=unbounded)"\
", visibility=SHADER_VISIBILITY_PIXEL),"\
"DescriptorTable("\
"   SRV(t0, space=1, numDescriptors=unbounded)"\
", visibility=SHADER_VISIBILITY_PIXEL),"\
"StaticSampler(s0)"
