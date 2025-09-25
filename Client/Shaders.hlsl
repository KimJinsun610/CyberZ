struct MATERIAL
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular; //a = power
	float4					m_cEmissive;
};

cbuffer cbCameraInfo : register(b1)
{
	matrix					gmtxView : packoffset(c0);
	matrix					gmtxProjection : packoffset(c4);
	float3					gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix					gmtxGameObject : packoffset(c0);
	MATERIAL				gMaterial : packoffset(c4);
	uint					gnTexturesMask : packoffset(c8);
};


#include "Light.hlsl"


struct CB_TO_LIGHT_SPACE
{
    matrix mtxToTextureSpace;
    float4 f4Position;
};

cbuffer cbToLightSpace : register(b6)
{
    CB_TO_LIGHT_SPACE gcbToLightSpaces[MAX_LIGHTS];
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

Texture2D gtxtAlbedoTexture : register(t6);
Texture2D gtxtSpecularTexture : register(t7);
Texture2D gtxtNormalTexture : register(t8);
Texture2D gtxtMetallicTexture : register(t9);
Texture2D gtxtEmissionTexture : register(t10);
Texture2D gtxtDetailAlbedoTexture : register(t11);
Texture2D gtxtDetailNormalTexture : register(t12);

SamplerState gssWrap : register(s0);

struct VS_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 uv : TEXCOORD;
    
    float4 shadowMapUVs[MAX_LIGHTS] : TEXCOORD1;
    
};

VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

    float4 positionW = mul(float4(input.position, 1.0f), gmtxGameObject);
    output.positionW = positionW.xyz;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (gcbToLightSpaces[i].f4Position.w != 0.0f) 
            output.shadowMapUVs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTextureSpace);
    }

	return(output);
}

float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);

	float3 normalW;
    float4 cColor = (cAlbedoColor * 1.0)+ (cSpecularColor * 1.0) + (cMetallicColor * 0.1) + (cEmissionColor * 10.0);
    cColor.a *= cAlbedoColor.a;
	
	if (gnTexturesMask & MATERIAL_NORMAL_MAP)
	{
		float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
		float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] ¡æ [-1, 1]
		normalW = normalize(vNormal);
	}
	else
	{
		normalW = normalize(input.normalW);
	}
    
    
    float4 cIllumination = Lighting(input.positionW, normalize(input.normalW), true, input.shadowMapUVs);
	return(lerp(cColor, cIllumination, 0.5f));
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			256

cbuffer cbBoneOffsets : register(b7)
{
	float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b8)
{
	float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

struct VS_SKINNED_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	int4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
};

VS_STANDARD_OUTPUT VSSkinnedAnimationStandard(VS_SKINNED_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;


	float4x4 mtxVertexToBoneWorld = (float4x4)0.0f;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
	}
    float4 positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld);
    output.positionW = positionW.xyz;
	output.normalW = mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
	output.tangentW = mul(input.tangent, (float3x3)mtxVertexToBoneWorld).xyz;
	output.bitangentW = mul(input.bitangent, (float3x3)mtxVertexToBoneWorld).xyz;

	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

     
    for (int j = 0; j < MAX_LIGHTS; j++)
    {
        if (gcbToLightSpaces[j].f4Position.w != 0.0f) 
            output.shadowMapUVs[j] = mul(positionW, gcbToLightSpaces[j].mtxToTextureSpace);
    }
    
	return(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
    float4 color : SV_TARGET0;

    float4 cTexture : SV_TARGET1;
    float4 cIllumination : SV_TARGET2;
    float4 normal : SV_TARGET3;
    float depth : SV_TARGET4;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedLightingToMultipleRTs(VS_STANDARD_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

    float2 uv = float2(input.uv);

    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
        cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
    
    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_SPECULAR_MAP)
        cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
    
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
    
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_METALLIC_MAP)
        cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
    
    float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_EMISSION_MAP)
        cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);

    float4 cColor = (cAlbedoColor * 1.0) + (cSpecularColor * 0.3) + (cMetallicColor * 0.1) + (cEmissionColor * 1.0);
    cColor.a *= cAlbedoColor.a;
    output.color = cColor;

    output.cTexture = cColor;
    
    float3 normalW;

    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
    {
        float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
        float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] ¡æ [-1, 1]
        normalW = normalize(vNormal);
    }
    else
    {
        normalW = normalize(input.normalW);
    }

    //output.normal = float4(normalW * 0.5f + 0.5f, 1.0f); // Convert to [0, 1]
    
    float4 normalLight = Light(input.positionW, normalize(normalW));
    float4 lightWithShadows = Lighting(input.positionW, 1, true, input.shadowMapUVs);
    
    output.cIllumination = lightWithShadows;
        
    output.normal = normalLight;
    output.depth = input.positionW.z;
    
    
    return output;
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTerrainBaseTexture : register(t1);
Texture2D gtxtTerrainDetailTexture : register(t2);

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET1
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gssWrap, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gssWrap, input.uv1);
//	float4 cColor = saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));
	float4 cColor = input.color * saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));
    cColor.a = 1.0;
	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.positionL = input.position;

	return(output);
}

TextureCube gtxtSkyCubeTexture : register(t13);
SamplerState gssClamp : register(s1);

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET1
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gssClamp, input.positionL);

	return(cColor);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct VS_BOUNDINGBOX_INPUT
{
    float3 position : POSITION;
    float4 colorIn : COLOR;
};

struct VS_BOUNDINGBOX_OUTPUT
{
    float4 positionH : SV_POSITION;
    float4 color : COLOR;
};

VS_BOUNDINGBOX_OUTPUT VSBoundingBox(VS_BOUNDINGBOX_INPUT input)
{
    VS_BOUNDINGBOX_OUTPUT output;
    float3 pos = { input.position.x, input.position.y, input.position.z };
    output.positionH = mul(mul(float4(pos, 1.0f), gmtxView), gmtxProjection);
    output.color = input.colorIn;
    return (output);
}

float4 PSBoundingBox(VS_BOUNDINGBOX_OUTPUT input) : SV_TARGET1
{
    return (input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

Texture2D gtxtParticleTexture : register(t25);
//SamplerState gssSet : register(s3);
struct VS_PARTICLE_INPUT
{
    float3 position : POSITION;
    float3 direction : DIRECTION;
    float  speed : SPEED;
    float  lifeTime : LIFETIME;
    float  time : TIME;
};

struct VS_PARTICLE_OUTPUT
{
    float4 positionH : SV_POSITION;
    float2 uv : COLOR;
    float lifeTime : LIFETIME;
    float time : TIME;
};

#define LENGTH 1.0f

VS_PARTICLE_OUTPUT VSParticle(VS_PARTICLE_INPUT input)
{
    
    VS_PARTICLE_OUTPUT output;
    float3 velocity = input.direction * (10.f* (sin(input.speed * input.time * input.time))*10);
    float3 pos = input.position;
    //float3 pos = /*{ input.position.x+velocity.x,input.position.y+velocity.y, input.position.z+velocity.z };*/input.position + velocity;
    //if (velocity.x * velocity.y * velocity.z <= 0)
    //{
    //    pos = input.position;
        
    //}
   // float3 gravity = float3(0.f, -20.f, 0.f);
   // velocity = velocity * gravity;

    //float3 up = (gmtxView._13, gmtxView._23, gmtxView._33);
    //float3 look = (gmtxView._12, gmtxView._22, gmtxView._32);
    //float3 right = (gmtxView._11, gmtxView._21, gmtxView._31);
    //up = normalize(cross(look, right));
    
    //float hw = 5.0f * LENGTH;
    //float hh = 5.0f * LENGTH;
    
    //float4 position[4] =
    //{
    //    float4(pos + (hw * right) - (hh * up), 1.0f), // LB
    //    float4(pos + (hw * right) + (hh * up), 1.0f), // LT
    //    float4(pos - (hw * right) - (hh * up), 1.0f), // RB
    //    float4(pos - (hw * right) + (hh * up), 1.0f) // RT 
    //};
    
    //float2 uv[4] =
    //{
    //    float2(0.0f, 1.0f),
    //    float2(0.0f, 0.0f),
    //    float2(1.0f, 1.0f),
    //    float2(1.0f, 0.0f)
    //};
    //for (int i = 0; i < 4; ++i)
    //{
    output.positionH = mul(mul(float4(pos,1.f), gmtxView), gmtxProjection);
    output.uv = float2(0, 0);
    output.lifeTime = input.lifeTime;
    output.time = input.time;
   // }
    return (output);
}

float4 PSParticle(VS_PARTICLE_OUTPUT input) : SV_TARGET1
{
    //float4 cColor = gtxtParticleTexture.Sample(gssWrap, input.positionH.xy);
    float4 cColor = float4(0.8f, 0.2f, 0.1, 1);
    return (cColor);
    //return (input.color);

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct PS_DEPTH_OUTPUT
{
    float fzPosition : SV_Target;
    float fDepth : SV_Depth;
};

PS_DEPTH_OUTPUT PSDepthWriteShader(VS_STANDARD_OUTPUT input)
{
    PS_DEPTH_OUTPUT output;
    
    float fNormalizedDepth = input.position.z / input.position.w;
    
    output.fzPosition = fNormalizedDepth;

    output.fDepth = input.position.z;

    return (output);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D<float4> gtxtTextureTexture : register(t14);
Texture2D<float4> gtxtIlluminationTexture : register(t15);
Texture2D<float4> gtxtNormalTexture2 : register(t16);

Texture2D<float> gtxtzDepthTexture : register(t17);
Texture2D<float> gtxttemp : register(t18);


struct VS_TEXTURED_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//
float4 VSPostProcessing(uint nVertexID : SV_VertexID) : SV_POSITION
{
    if (nVertexID == 0)
        return (float4(-1.0f, +1.0f, 0.0f, 1.0f));
    if (nVertexID == 1)
        return (float4(+1.0f, +1.0f, 0.0f, 1.0f));
    if (nVertexID == 2)
        return (float4(+1.0f, -1.0f, 0.0f, 1.0f));

    if (nVertexID == 3)
        return (float4(-1.0f, +1.0f, 0.0f, 1.0f));
    if (nVertexID == 4)
        return (float4(+1.0f, -1.0f, 0.0f, 1.0f));
    if (nVertexID == 5)
        return (float4(-1.0f, -1.0f, 0.0f, 1.0f));

    return (float4(0, 0, 0, 0));
}

float4 PSPostProcessing(float4 position : SV_POSITION) : SV_Target
{
    return (float4(0.0f, 0.0f, 0.0f, 1.0f));
}

///////////////////////////////////////////////////////////////////////////////
//

struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_SCREEN_RECT_TEXTURED_OUTPUT VSScreenRectSamplingTextured(uint nVertexID : SV_VertexID)
{
    VS_SCREEN_RECT_TEXTURED_OUTPUT output = (VS_SCREEN_RECT_TEXTURED_OUTPUT) 0;

    if (nVertexID == 0)
    {
        output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 0.0f);
    }
    else if (nVertexID == 1)
    {
        output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 0.0f);
    }
    else if (nVertexID == 2)
    {
        output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 1.0f);
    }
    else if (nVertexID == 3)
    {
        output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 0.0f);
    }
    else if (nVertexID == 4)
    {
        output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 1.0f);
    }
    else if (nVertexID == 5)
    {
        output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 1.0f);
    }

    return (output);
}


cbuffer cbDrawOptions : register(b5)
{
    int4 gvDrawOptions : packoffset(c0);
};

float3 ReconstructPosition(float2 uv, float depth)
{
    float4 clipSpacePosition;
    clipSpacePosition.xy = uv * 2.0 - 1.0; // NDC
    clipSpacePosition.z = depth;
    clipSpacePosition.w = 1.0;

    float4 viewSpacePosition = mul((gmtxProjection), clipSpacePosition);
    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}


float4 PSScreenRectSamplingTextured(VS_SCREEN_RECT_TEXTURED_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;

    // Sample G-Buffer
    float4 color = gtxtTextureTexture.Sample(gssWrap, uv);
    float4 normallight = gtxtNormalTexture2.Sample(gssWrap, uv);
    float4 specular = gtxtSpecularTexture.Sample(gssWrap, uv);
    float depth = gtxtDepthTextures[0].Sample(gssWrap, input.uv).r;
        
    // Combine albedo and light
    float4 light = gtxtIlluminationTexture.Sample(gssWrap, uv);
    
    float4 outcolor = lerp(color, normallight, 0.4f);
   // outcolor = lerp(color, light, 0.5f);
    if (light.x < 0.2) outcolor = outcolor - 0.4;
     
    return (outcolor);
    
}

