// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// A switch which allows you to disable the auto-scaling and revert to normal scaling.
uniform float UTO_OrthographicZoom;
// A switch which allows you to choose orthographic mode.
uniform float UTO_OrthographicMode;
uniform vec4 UTO_Viewport;
uniform mat4 osg_ModelViewMatrix;
uniform mat4 osg_ViewMatrix;
uniform mat4 osg_ViewMatrixInverse;
uniform mat4 osg_ProjectionMatrix;
uniform mat3 osg_NormalMatrix;
uniform mat4 osg_ModelViewProjectionMatrix;
uniform float VA_TrueScale;
uniform float VA_IconScale;
uniform float VA_ScreenScale;

vec4 GetModelZero()
{
   mat4 osg_ModelMatrix = osg_ViewMatrixInverse * osg_ModelViewMatrix;
   vec4 wsmodelzero = osg_ModelMatrix * vec4(0.0, 0.0, 0.0, 1.0);
   return osg_ViewMatrix * vec4(normalize(wsmodelzero.xyz), 1.0);
}

float GetScreenScalar()
{
   // Transform only the (object) origin to clip-space
   vec4 zero = osg_ModelViewProjectionMatrix * vec4(0.0, 0.0, 0.0, 1.0);
   // The w component of a clip-space vertex gives you information about the scaling due to perspective.
   // Here, we extract the w component of the object origin and manipulate it.
   // Orthographic projection works differently, and UTO_OrthographicMode lets you switch between 
   // perspective and orthographic.
   return mix(min(UTO_Viewport.w / zero.w / VA_IconScale * VA_ScreenScale * 0.1, 1.0), 1.0 / UTO_OrthographicZoom / VA_IconScale * VA_ScreenScale * 0.1, UTO_OrthographicMode);
}

vec4 ScreenSpaceTransform(vec4 aInput)
{
   // manipulate the scalar component to cause screen scaling during the usual transform.
   // trueScale allows you to turn off the scalar to the normal fixed pipeline stuff.
   float scalar = mix( GetScreenScalar(), aInput.w, VA_TrueScale); 
   
   return osg_ModelViewProjectionMatrix * vec4(aInput.xyz, scalar);
}

vec4 BillboardTransform(vec4 aInput)
{
   // manipulate the scalar component to cause screen scaling during the usual transform.
   // trueScale allows you to turn off the scalar to the normal fixed pipeline stuff.
   float scalar = mix(GetScreenScalar(), aInput.w, VA_TrueScale);
   
   mat4 mm = osg_ModelViewMatrix;
   float sx = length(mm[0].xyz);
   float sy = length(mm[1].xyz);
   float sz = length(mm[2].xyz);
   mm[0][0] = 0.0;
   mm[0][1] = sx;
   mm[0][2] = 0.0;
   mm[1][0] = sy;
   mm[1][1] = 0.0;
   mm[1][2] = 0.0;
   mm[2][0] = 0.0;
   mm[2][1] = 0.0;
   mm[2][2] = sz;
   
   // Here we combine the manipulated scalar component with the original xyz and perform the usual transform.
   return osg_ProjectionMatrix * mm * vec4(aInput.xyz, scalar);
}
