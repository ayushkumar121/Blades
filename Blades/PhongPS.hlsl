cbuffer LightCBuf // Global Light Properties
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;

	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;

};

cbuffer ObjectCBuf // Material Properties
{
	float3 materialColor;
	float specularIntensity;
	float specularPower;
};

float4 main(float3 worldPos : Position, float3 n : Normal) : SV_Target
{
	// fragment to light vector data
	const float3 vToL = lightPos - worldPos;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;


	//  attenuation
	const float att = 1 / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
	//float3 diffuse = saturate(dot(viewNormal, vToL)) * materialColor;
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));

	const float3 w = n * dot(vToL, n);
	const float3 r = w * 2.0f - vToL;
	// specular highlights

	const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);

	// final color
	return float4(saturate( (diffuse + ambient + specular) * materialColor), 1.0f);
}