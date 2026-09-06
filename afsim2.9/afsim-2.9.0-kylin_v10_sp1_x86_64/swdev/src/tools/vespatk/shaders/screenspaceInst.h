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
uniform float VA_TrueScale;
uniform float VA_IconScale;
uniform float VA_ScreenScale;
uniform mat4 osg_ProjectionMatrix;
uniform mat4 instanceModelMatrix[50];

vec4 GetModelZero()
{
   vec4 wsmodelzero = instanceModelMatrix[gl_InstanceID] * vec4(0.0, 0.0, 0.0, 1.0);
   return vec4(normalize(wsmodelzero.xyz), 1.0);
}

vec4 WorldTransform(vec4 aInput)
{
	return instanceModelMatrix[gl_InstanceID] * aInput;
}

float GetScreenScalar()
{
   // Transform only the (object) origin to clip-space
   vec4 zero = osg_ProjectionMatrix * instanceModelMatrix[gl_InstanceID] * vec4(0.0, 0.0, 0.0, 1.0);

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
   
   return osg_ProjectionMatrix * instanceModelMatrix[gl_InstanceID] * vec4(aInput.xyz, scalar);
}

mat3 ScreenSpaceNormalMatrix()
{
   return mat3(transpose(inverse(instanceModelMatrix[gl_InstanceID])));
}

vec4 BillboardTransform(vec4 aInput)
{
   // manipulate the scalar component to cause screen scaling during the usual transform.
   // trueScale allows you to turn off the scalar to the normal fixed pipeline stuff.
   float scalar = mix(GetScreenScalar(), aInput.w, VA_TrueScale);

   mat4 mm = instanceModelMatrix[gl_InstanceID];
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
   vec4 v = osg_ProjectionMatrix * mm * vec4(aInput.xyz, scalar);
   return vec4(v.xy, 0.995 * v.z, v.w); // this bias (0.995) is to keep billboards from clipping in unpleasant ways... there is probably a better solution.
}
