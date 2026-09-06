// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform mat4 osg_ProjectionMatrix;
uniform mat4 osg_ModelViewMatrix;

in vec4 trace_Vertex;

//USE_FLAT_MAP_PROJECTION is set/unset by VaViewer::MapLoadedMapCB
#pragma import_defines (USE_FLAT_MAP_PROJECTION)

#ifdef USE_FLAT_MAP_PROJECTION
#include "CoordConv.h"
out float xpos;
#else
#include "logDepth.h" // logarithmic depth handing
uniform float uECI; // indicates we want to use ECI coords
uniform float uTime; // current time
#endif

#ifdef NO_GEOMETRY
flat out int oState;
#endif

out float aTime;

void main(void)
{
   aTime = trace_Vertex.w;
#ifdef NO_GEOMETRY
   oState = 0;
#endif

#ifdef USE_FLAT_MAP_PROJECTION
   vec4 flatMap = GetEqrCoords(trace_Vertex);
   xpos = flatMap.x;
   gl_Position = osg_ProjectionMatrix * osg_ModelViewMatrix * flatMap;
#else
   float earthAngle = 7.292115147e-5 * (aTime - uTime); // constant is earth's rot. rate (rad/sec)
   float sEA = sin(earthAngle);
   float cEA = cos(earthAngle);
   vec4 eciVert = vec4(cEA * trace_Vertex.x - sEA * trace_Vertex.y,
                       sEA * trace_Vertex.x + cEA * trace_Vertex.y,
                       trace_Vertex.z, 1.0);
   vec4 vert = mix(vec4(trace_Vertex.xyz, 1.0), eciVert, uECI);
   vec4 p = osg_ProjectionMatrix * osg_ModelViewMatrix * vert;
   vtkLogDepth(p, osg_ProjectionMatrix);
   gl_Position = p;
#endif
}
