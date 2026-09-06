// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#pragma import_defines (HIDE_BEHIND_EARTH)

uniform vec4 UTO_Viewport;
uniform float UTO_OrthographicZoom;
uniform float UTO_OrthographicMode;
uniform float VA_IconScale;
uniform float VA_TrueScale;
uniform vec4 offsetAndScale;
uniform mat4 osg_ViewMatrix;
in vec4 in_vertex;
in vec4 in_texCoord;
out float depth;
out vec2 texCoord;

const float cEarthRadius = 6378137.0;

#include "logDepth.h" // logarithmic depth handing

void main( void )
{
   // move it over and up by scale size
   vec4 zero = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(0.0, 0.0, 0.0, 1.0);

   float w = zero.w;
   depth = zero.z / zero.w;
   zero /= w; // normalize... this will happen later anyway, but we want normal coords for...
   zero.z *= 0.995;

   // apply the vertex as a pixel offset
   vec2 adjusted = vec2(in_vertex.xy * offsetAndScale.zw) + offsetAndScale.xy;
   vec4 offset = vec4(2.0 * adjusted.x / UTO_Viewport.z, 2.0 * adjusted.y / UTO_Viewport.w, 0.0, 0.0);

   vec4 p = zero + offset;
   vtkLogDepth(p, gl_ProjectionMatrix);
   gl_Position = p;
   texCoord = in_vertex.xy;
   
#ifdef HIDE_BEHIND_EARTH
   vec3 ec = vec4(osg_ViewMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;     // earth's center in view-space
   vec3 mc = vec4(gl_ModelViewMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz; // model's center in view-space
   float f = dot(mc, ec) / dot(mc, mc);
   if ((f > 0) && (f < 1))
   {
      vec3 c = mc * f - ec; // the vector closest to the world-space origin
      float d = dot(c, c);
      if (d < cEarthRadius * cEarthRadius)
      {
         depth = 2.0; // this will cause the fragments to be discarded
      }
   }
#endif
}
