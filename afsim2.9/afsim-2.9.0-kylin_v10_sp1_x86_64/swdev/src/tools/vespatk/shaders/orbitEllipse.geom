#version 150
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#pragma import_defines (USE_FLAT_MAP_PROJECTION) 

layout (lines) in;
layout (line_strip) out;
layout (line_strip) out;
layout (max_vertices = 204) out; // not sure what to put here // was 204 with just one out float + verts

const float cM_PER_DEGREE_EQ = 111318.163;

// CLIP_LON is a define set by the application, it is the longitude we want to cut these at.
// I am skipping it for now.

void main(void)
{
   int i;
   vec4 lastv = gl_in[0].gl_Position;
   for (i = 0; i < gl_in.length(); i++)
   {
      vec4 thisv = gl_in[i].gl_Position;
      
      #ifdef USE_FLAT_MAP_PROJECTION
      if (i > 0)
      {
         float x0 = gl_in[i - 1].gl_Position.x;
         float x1 = gl_in[i].gl_Position.x;
         // if we cross the date line or the pm
         if (x0 * x1 <= 0)
         {
            // if the last point was within 45 degrees of the date line 
            if (abs(x0) > 0.75 * 180.0 * cM_PER_DEGREE_EQ)
            {
               // offsetv is thisv in lastv's hemisphere
               vec4 offsetv = thisv;
               float factor = 0.0; // interpolation factor
               if (x0 < 0.0)
               {
                  offsetv -= vec4(360.0 * cM_PER_DEGREE_EQ, 0.0, 0.0, 0.0);
                  x1 -= 360.0 * cM_PER_DEGREE_EQ;
                  factor = (-180.0 * cM_PER_DEGREE_EQ - x0) / (x1 - x0);
               }
               else
               {
                  offsetv += vec4(360.0 * cM_PER_DEGREE_EQ, 0.0, 0.0, 0.0);
                  x1 += 360.0 * cM_PER_DEGREE_EQ;
                  factor = (180.0 * cM_PER_DEGREE_EQ - x0) / (x1 - x0);
               }
               vec4 interp = mix(offsetv, lastv, factor); // the interpolation point in lastv's hemisphere
               gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * interp;
               EmitVertex();
               EndPrimitive();
               lastv = interp * vec4(-1.0, 1.0, 1.0, 1.0); // the interpolation point in thisv's hemisphere
               gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * lastv;
               EmitVertex();
            }
         }
      }
      #endif

      gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * thisv;
      lastv = thisv;
      EmitVertex();
   }
   EndPrimitive();
}