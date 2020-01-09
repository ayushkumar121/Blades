Texture2D tex;
SamplerState splr;

float4 main(float2 tc : TexCoord) : SV_Target
{
	float4 dtex = tex.Sample(splr,tc);
	clip(dtex.a < 0.1f ? -1 : 1);
	return dtex;
}