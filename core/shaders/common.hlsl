#ifndef __BASIC_TYPES_HLSL__
#define __BASIC_TYPES_HLSL__

#ifdef __cplusplus
#ifndef CHECK_STRUCT_ALIGNMENT
#define CHECK_STRUCT_ALIGNMENT(s) static_assert( sizeof(s) % 16 == 0, "sizeof(" #s ") is not multiple of 16" )
#endif
#else
#ifndef CHECK_STRUCT_ALIGNMENT
#define CHECK_STRUCT_ALIGNMENT(s) 
#endif
#endif

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

#endif 