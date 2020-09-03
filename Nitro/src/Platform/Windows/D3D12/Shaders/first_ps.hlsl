struct PsIn
{
	float3 Color : COLOR;
};
float4 main(PsIn ps_in) : SV_TARGET
{
	return float4(normalize(ps_in.Color), 1.f);
}