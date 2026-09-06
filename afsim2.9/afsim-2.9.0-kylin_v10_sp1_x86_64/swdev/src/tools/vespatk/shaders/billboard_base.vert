// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform vec4 instanceTeamColor[50];
in vec4 model_Vertex;
in vec2 model_MultiTexCoord0;
out vec3 lightVec;
out vec3 eyeVec;
out vec4 teamColor;
out vec2 texCoord;

#include "screenspaceInst.h"

void main( void )
{
   teamColor = instanceTeamColor[gl_InstanceID];

   eyeVec = vec3(0, 0, -1);
   lightVec = vec3(0, 0, -1);

   texCoord = model_MultiTexCoord0;
   gl_Position = BillboardTransform(model_Vertex);
}
