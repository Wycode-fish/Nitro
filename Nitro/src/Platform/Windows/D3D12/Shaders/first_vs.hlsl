struct VsIn
{
	float3 Position : POSITION;
};

struct VsOut
{
	float3 Color : COLOR;
	float4 Position : SV_Position;
};

cbuffer VSContants : register(b0)
{
	float4 colorShifter;
};

VsOut main(VsIn vs_in)
{
	VsOut vs_out;

	vs_out.Position = float4(vs_in.Position, 1.0f);
	vs_out.Color = vs_in.Position + colorShifter.xyz;

	return vs_out;
}