// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform float VA_DirectionallyLit;
uniform float VA_ModelsSupportLighting;
uniform vec4 VA_TeamColor;
uniform vec3 vtk_sunVec;
in vec4 model_Vertex;
in vec3 model_Normal;
in vec4 model_MultiTexCoord0;
out vec3 lightVec;
out vec3 eyeVec;
out vec4 teamColor;
out vec2 texCoord;

#include "screenspace300.h"
#include "logDepth.h" // logarithmic depth handing

out vec3 oNormal;
out vec3 oLight;
out vec3 oFragPos;
out vec2 oTexCoord;
out vec3 oViewPos;

void main( void )
{
   vec4 ssp = ScreenSpaceTransform(model_Vertex);
   oFragPos = vec3(ssp);
   vtkLogDepth(ssp, osg_ProjectionMatrix);
   gl_Position = ssp;
   oTexCoord = vec2(model_MultiTexCoord0);
   oNormal = vec3(osg_ViewMatrixInverse * osg_ModelViewMatrix * vec4(model_Normal, 0.0));
   oLight = vtk_sunVec;
   oViewPos = vec3(osg_ViewMatrix * vec4(0.0, 0.0, 0.0, 1.0));
}
