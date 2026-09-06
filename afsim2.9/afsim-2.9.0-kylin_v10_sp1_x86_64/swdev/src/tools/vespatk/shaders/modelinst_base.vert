// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform float VA_DirectionallyLit;
uniform vec4 instanceTeamColor[50];
uniform vec3 vtk_sunVec;
uniform mat4 osg_ViewMatrix;
uniform float VA_ModelsSupportLighting;
in vec4 model_Vertex;
in vec3 model_Normal;
in vec4 model_MultiTexCoord0;
out vec3 lightVec;
out vec3 eyeVec;
out vec4 teamColor;
out vec2 texCoord;

#include "screenspaceInst.h" // model view xforms
#include "logDepth.h" // logarithmic depth handing

void main( void )
{
   vec4 sun = osg_ViewMatrix * vec4(vtk_sunVec, 0.0);
   teamColor = instanceTeamColor[gl_InstanceID];
   vec4 vsmodelzero = GetModelZero();

   vec4 p = ScreenSpaceTransform(model_Vertex);
   vtkLogDepth(p, osg_ProjectionMatrix);
   gl_Position = p;
   
   texCoord = vec2(model_MultiTexCoord0);
   eyeVec = vec3(WorldTransform(model_Vertex));

   mat3 normalMatrix = ScreenSpaceNormalMatrix();
   vec3 n = normalize(normalMatrix * model_Normal.xyz);
   vec3 t = cross(n, vec3(0.0, 0.0, 1.0));
   t = normalize(t);
   vec3 b = normalize(cross(n, t));
   vec3 v;
   vec3 ls = mix(-vsmodelzero.xyz, sun.xyz, VA_DirectionallyLit * VA_ModelsSupportLighting);
   v.x = dot(ls, t);
   v.y = dot(ls, b);
   v.z = dot(ls, n);
   lightVec = normalize(v);
   v.x = dot(eyeVec, t);
   v.y = dot(eyeVec, b);
   v.z = dot(eyeVec, n);
   eyeVec = normalize(v);
}
