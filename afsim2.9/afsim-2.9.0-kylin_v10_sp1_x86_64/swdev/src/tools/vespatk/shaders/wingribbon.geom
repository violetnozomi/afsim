#version 150
#extension GL_ARB_shader_storage_buffer_object : enable
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "logDepth.h" // logarithmic depth handing

layout (lines) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out; // not sure what to put here // was 204 with just one out float + verts

in vec3 vWingTipA[];
in vec3 vWingTipB[];
in float vStateValMain[];
in float vStateValEdge[];
in float vIsAlive[];

//uniform mat4 osg_ViewMatrix;
uniform mat4 osg_ProjectionMatrix;
//uniform mat4 osg_ModelViewMatrix;
uniform mat4 osg_ModelViewProjectionMatrix;

uniform float maxMainLimit;
uniform float minMainLimit;
uniform float maxEdgeLimit;
uniform float minEdgeLimit;

uniform float wingSpanScalar;

smooth out float oStateMain;
smooth out float oStateEdge;
smooth out float edgeSide;
noperspective out float oIsAlive;

//USE_FLAT_MAP_PROJECTION is set/unset in VaViewer::MapLoadedMapCB
#pragma import_defines (USE_FLAT_MAP_PROJECTION, HIGHLIGHTING) 
#ifdef USE_FLAT_MAP_PROJECTION
#include "CoordConv.h"
#endif
#ifdef HIGHLIGHTING
#define EXTRA_WIDTH_FOR_HIGHLIGHT 0.35f
#define HIGHLIGHTED_WIDTH         1.35f
uniform vec4 highlightColor;
#endif
void main(void)
{   
   #ifdef USE_FLAT_MAP_PROJECTION
   // untested section of code
   // caused when model wraps around to other side of map, we don't want a giant streak being created across the entire map
   // if we cross the date line or the pm
   if (GetEqrCoords(gl_in[0].gl_Position).x * GetEqrCoords(gl_in[1].gl_Position).x <= 0){
      // if the last point was within 45 degrees of the date line 
       if (abs(GetEqrCoords(gl_in[0].gl_Position).x ) > 0.75 * 3.14159 * 6371000){
         return;
       }
   }      
   #endif
   
   
   int i;
   for( i = 0; i < gl_in.length(); i++)
   {
      
      vec3 wingTipA = vWingTipA[i];
      vec3 wingTipB = vWingTipB[i];
      
      vec3 wingTipsMidpoint=(wingTipA+wingTipB)/2;
      
      #ifdef HIGHLIGHTING
      //highlighting is on      
      //make the wingspan wider to account for the additional highlighting edge
      wingTipA*=HIGHLIGHTED_WIDTH;
      wingTipB*=HIGHLIGHTED_WIDTH;      
      #endif
      
      wingTipA = wingTipsMidpoint+(wingTipA-wingTipsMidpoint)*wingSpanScalar;
      wingTipB = wingTipsMidpoint+(wingTipB-wingTipsMidpoint)*wingSpanScalar;
      
      
      vec4 vert=gl_in[i].gl_Position;
      vec4 cornerA=(vert+vec4(wingTipA,0));
      vec4 cornerB=(vert+vec4(wingTipB,0));
      
      #ifdef USE_FLAT_MAP_PROJECTION
      cornerA = GetEqrCoords(cornerA);
      cornerB = GetEqrCoords(cornerB);   
      //todo should I add an additional check for crossing prime meridian (warping to other side of map) here?
      // if we cross the date line or the pm
      if (cornerA.x * cornerB.x <= 0)
      {
         // if the last point was within 45 degrees of the date line 
         if (abs(cornerA.x) > 0.75 * 3.14159 * 6371000)
         {
            return;
         }
      }      
      #endif
      
      oStateMain = (clamp(vStateValMain[i],minMainLimit,maxMainLimit)-minMainLimit)/(maxMainLimit-minMainLimit);
      oStateEdge = (clamp(vStateValEdge[i],minEdgeLimit,maxEdgeLimit)-minEdgeLimit)/(maxEdgeLimit-minEdgeLimit);
      oIsAlive = vIsAlive[i];
      vec4 p = osg_ModelViewProjectionMatrix*cornerB;
      vtkLogDepth(p, osg_ProjectionMatrix);
	  gl_Position = p;
      edgeSide = 
         #ifdef HIGHLIGHTING
         (true) ? HIGHLIGHTED_WIDTH :
         #endif
         1.0f;
      EmitVertex();
	  p = osg_ModelViewProjectionMatrix*cornerA;
	  vtkLogDepth(p, osg_ProjectionMatrix);
      gl_Position = p;
      edgeSide = 
         #ifdef HIGHLIGHTING
         (true) ? -HIGHLIGHTED_WIDTH :
         #endif
         -1.0f;     
      EmitVertex();
   }
   
   EndPrimitive();      
}
