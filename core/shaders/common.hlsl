#ifndef __BASIC_TYPES_HLSL__
#define __BASIC_TYPES_HLSL__

#ifdef __cplusplus
    #ifndef CHECK_STRUCT_ALIGNMENT
    #define CHECK_STRUCT_ALIGNMENT(s) static_assert( sizeof(s) % 16 == 0, "sizeof(" #s ") is not multiple of 16" )
    #endif

    #ifndef _ATTRIB
    #define _ATTRIB(x)
    #endif
#else
    #ifndef CHECK_STRUCT_ALIGNMENT
    #define CHECK_STRUCT_ALIGNMENT(s) 
    #endif

    #ifndef _ATTRIB
    #define _ATTRIB(x) : x
    #endif
#endif

#define Z_FLIP 1.0

// Input structures shared with C++ code
struct CameraAttribs {
    float4x4 mView;
    float4x4 mViewProj;
    float4x4 mProj;
    float4x4 mInvView;
    float4x4 mInvViewProj;
    float4x4 mInvProj;

    float2 mViewport;
    float mNearZ;
    float mFarZ;

    float3 mPosition;
    float mPadding0;

    float3 mViewDirection;
    float mPadding1;
};
CHECK_STRUCT_ALIGNMENT(CameraAttribs);

struct SceneGlobals {
    CameraAttribs mCamera;
};
CHECK_STRUCT_ALIGNMENT(SceneGlobals);

struct StaticInstanceData {
    float4x4 mWorld;
    int mEntity;
    int mPadding0;
    int mPadding1;
    int mPadding2;
};
CHECK_STRUCT_ALIGNMENT(StaticInstanceData);

struct SpriteBatchVSInput
{
    // Use W component as rotation
    float4 mPos         _ATTRIB(ATTRIB0);
    float4 mColor       _ATTRIB(ATTRIB1);

    float2 mUVTop       _ATTRIB(ATTRIB2);
    float2 mUVBottom    _ATTRIB(ATTRIB3);

    float2 mSize        _ATTRIB(ATTRIB4);
    float2 mOrigin      _ATTRIB(ATTRIB5);
};
CHECK_STRUCT_ALIGNMENT(SpriteBatchVSInput);

#endif 