#version 120
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

//USE_FLAT_MAP_PROJECTION is set/unset by VaViewer::MapLoadedMapCB
#pragma import_defines (USE_FLAT_MAP_PROJECTION)
varying float aTime;
uniform float uECI; // indicates we want to use ECI coords
uniform float uTime; // current time

#define vertex gl_Vertex
#define time gl_Vertex.w
#define wingTipA gl_Color.xyz
#define stateValMain gl_Color.w
#define wingTipB gl_Normal
#define stateValEdge gl_MultiTexCoord0.r
#define isAlive gl_MultiTexCoord0.g

#define oPosition gl_Position
#define oWingTipA vWingTipA
#define oWingTipB vWingTipB
#define oStateValMain vStateValMain
#define oStateValEdge vStateValEdge
#define oIsAlive vIsAlive

varying vec3 vWingTipA;
varying vec3 vWingTipB;
varying float vStateValMain;
varying float vStateValEdge;
varying float vIsAlive;

void main(void)
{
   oStateValEdge=stateValEdge;
   oStateValMain=stateValMain;
   oWingTipA=wingTipA;
   oWingTipB=wingTipB;
   oIsAlive = isAlive;
   #ifdef USE_FLAT_MAP_PROJECTION
   vec4 vert = vec4(gl_Vertex.xyz,1);
   oPosition=vert;
   #else
   float earthAngle = 7.292115147e-5 * (time - uTime); // constant is earth's rot. rate (rad/sec)
   float sEA = sin(earthAngle);
   float cEA = cos(earthAngle);
   vec4 eciVert = vec4(cEA * gl_Vertex.x - sEA * gl_Vertex.y,
                       sEA * gl_Vertex.x + cEA * gl_Vertex.y,
                       gl_Vertex.z, 1);
   vec4 vert = mix(vec4(gl_Vertex.xyz,1), eciVert, uECI);
   oPosition=vert;
   #endif
}
