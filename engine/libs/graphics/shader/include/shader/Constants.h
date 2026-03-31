#if !defined(SLUG_SHADER_COMPILE)

#pragma once
#include "shader/Define.h"
#else
#include "Define.h"
#endif //!defined(SLUG_SHADER_COMPILE)


#if !defined(SLUG_SHADER_COMPILE)
namespace slug
{
namespace shader
{
#endif //!defined(SLUG_SHADER_COMPILE)

SLUG_CBUFFER(SceneConstants, SLUG_SHADER_REGISTER_SCENE_CB)
{
    SLUG_SHADER_FLOAT(frameCount)
    SLUG_SHADER_FLOAT(deltaTime)
};

SLUG_CBUFFER(ViewConstants, SLUG_SHADER_REGISTER_VIEW_CB)
{
    SLUG_SHADER_MATRIX4x4(view)
    SLUG_SHADER_MATRIX4x4(projection)
    SLUG_SHADER_MATRIX4x4(viewProjection)
    SLUG_SHADER_MATRIX4x4(invView)
    SLUG_SHADER_MATRIX4x4(invProjection)
    SLUG_SHADER_MATRIX4x4(invViewProjection)
    SLUG_SHADER_MATRIX4x4(preViewProjection)
    SLUG_SHADER_MATRIX4x4(preInvViewProjection)
    SLUG_SHADER_FLOAT4(cameraPosition)
    SLUG_SHADER_FLOAT2(resolution)
    SLUG_SHADER_FLOAT2(invResolution)
};

SLUG_CBUFFER(ObjectConstants, SLUG_SHADER_REGISTER_OBJECT_CB)
{
    SLUG_SHADER_MATRIX4x4(world)
    SLUG_SHADER_MATRIX4x4(adjoint)
};

#if !defined(SLUG_SHADER_COMPILE)
}
}
#endif !defined(SLUG_SHADER_COMPILE)
