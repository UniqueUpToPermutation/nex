#include "common.hlsl"

cbuffer cbContextData : register(b0)
{
    SceneGlobals gGlobals;
};

struct SpriteBatchGSInput
{
	float4 mPos  		: SV_Position;
	float4 mColor		: COLOR0;
	float4 mUVX			: TEXCOORD0;
	float4 mUVY			: TEXCOORD1;
	float2 mUVTop		: TEXCOORD2;
	float2 mUVBottom	: TEXCOORD3;
};

struct SpriteBatchPSInput 
{
	float4 mPos 		: SV_Position;
	float4 mColor		: COLOR0;
	float2 mUV			: TEXCOORD0;
};

/*
    Vertex Shader
*/
#if defined(VERTEX_SHADER)

void main(in SpriteBatchVSInput VSIn,
    out SpriteBatchGSInput GSIn) 
{
    float4 pos = float4(VSIn.mPos.xyz, 1.0);
	float rotation = VSIn.mPos.w;

	float2 uvSlice = VSIn.mUVBottom - VSIn.mUVTop;

	float cosRot = cos(rotation);
	float sinRot = sin(rotation);

	float2x2 rotMat = float2x2(
		cosRot, sinRot, 
		-sinRot, cosRot);

	float2 halfSize = VSIn.mSize / 2;

	float2 uvx = float2(halfSize.x, 0.0);
	float2 uvy = float2(0.0, halfSize.y);

	uvx = mul(rotMat, uvx);
	uvy = mul(rotMat, uvy);

	float2 toCenter = halfSize - VSIn.mOrigin;

	toCenter = mul(rotMat, toCenter);

	pos.xy += toCenter;

	GSIn.mPos = mul(pos, gGlobals.mCamera.mViewProj);
	GSIn.mUVX = mul(float4(uvx, 0.0, 0.0), gGlobals.mCamera.mViewProj);
	GSIn.mUVY = mul(float4(uvy, 0.0, 0.0), gGlobals.mCamera.mViewProj);

	GSIn.mUVTop = VSIn.mUVTop;
	GSIn.mUVBottom = VSIn.mUVBottom;
	GSIn.mColor = VSIn.mColor;
}

#endif

/*
    Geometry Shader
*/
#if defined(GEOMETRY_SHADER)

[maxvertexcount(4)]
void main(point SpriteBatchGSInput input[1], 
	inout TriangleStream<SpriteBatchPSInput> triStream)
{
    SpriteBatchPSInput v;
    v.mColor = input[0].mColor;

	float2 uvTL = input[0].mUVTop;
	float2 uvBR = input[0].mUVBottom;
	float2 uvTR = float2(uvBR.x, uvTL.y);
	float2 uvBL = float2(uvTL.x, uvBR.y);

	float4 left = input[0].mPos - input[0].mUVX;
	float4 right = input[0].mPos + input[0].mUVX;
	float4 pTL = left + input[0].mUVY;
	float4 pTR = right + input[0].mUVY;
	float4 pBL = left - input[0].mUVY;
	float4 pBR = right - input[0].mUVY;
 
    // create sprite quad
    // bottom left
	v.mPos = pBL;
	v.mUV = uvBL;
    triStream.Append(v);
 
    // top left
    v.mPos = pTL;
	v.mUV = uvTL;
    triStream.Append(v);
 
    // bottom right
	v.mPos = pBR;
	v.mUV = uvBR;
    triStream.Append(v);
 
    // top right
	v.mPos = pTR;
	v.mUV = uvTR;
    triStream.Append(v);
}

#endif
/*
    Fragment Shader
*/
#if defined(PIXEL_SHADER)

Texture2D mTexture;
SamplerState mTexture_sampler;

void main(in SpriteBatchPSInput PSIn,
	out float4 Color : SV_TARGET) {

	Color = mTexture.Sample(mTexture_sampler, PSIn.mUV) * PSIn.mColor;
}

#endif