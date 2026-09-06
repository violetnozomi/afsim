#version 150
#extension GL_ARB_shader_storage_buffer_object : enable

#pragma import_defines (USE_FLAT_MAP_PROJECTION) 
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

layout (lines) in;
layout (line_strip) out;
layout (max_vertices = 204) out; // not sure what to put here // was 204 with just one out float + verts


#ifdef USE_FLAT_MAP_PROJECTION
in float xpos[];
#endif
in float aTime[];

flat out int oState;

struct timeState
{
   float time;
   float state;
};

layout (std430, binding=3) buffer state_data
{
   timeState state[];
};

uniform ivec3 stateCircular; // start, size, buffersize

vec2 CircularAccess(int aIndex)
{
   int start = stateCircular.x;
   int size = stateCircular.y;
   int bsize = stateCircular.z;
   if (aIndex >= size)
   {
      return vec2(-1.0, -1.0);
   }
   int offset = aIndex + start;
   if (offset >= bsize)
   {
      offset = offset - bsize;
   }
   return vec2(state[offset].time, state[offset].state);
}

// CLIP_LON is a define set by the application, it is the longitude we want to cut these at.
// I am skipping it for now.

void main(void)
{
   int stateIndex = 0;
   int i;
   oState = 0;
   float lastTime = aTime[0];
   vec4 lastv = gl_in[0].gl_Position;
   for (i = 0; i < gl_in.length(); i++)
   {
      vec4 thisv = gl_in[i].gl_Position;
      float thisTime = aTime[i];
      
      #ifdef USE_FLAT_MAP_PROJECTION
      if (i > 0)
      {
         // if we cross the date line or the pm
         if (xpos[i-1] * xpos[i] <= 0)
         {
            // if the last point was within 45 degrees of the date line 
            if (abs(xpos[i-1]) > 0.75 * 3.14159 * 6371000)
            {
               EndPrimitive();
            }
         }
      }
      #endif
      
      vec2 stateAcc = CircularAccess(stateIndex);
      while ((stateAcc.x >= 0.0) && (thisTime >= stateAcc.x))
      {
         if (stateAcc.x > lastTime)
         {
            gl_Position = mix(lastv, thisv, (stateAcc.x - lastTime) / (thisTime - lastTime));
            EmitVertex();
         }

         oState = int(stateAcc.y);
         ++stateIndex;
         stateAcc = CircularAccess(stateIndex);
      }

      lastTime = thisTime;
      gl_Position = thisv;
      lastv = thisv;
      EmitVertex();
   }
   EndPrimitive();
}