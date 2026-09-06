#version 150
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
layout (max_vertices = 4) out;
uniform mat4 osg_ViewMatrix;

in vec3 ecefCoord[];
const float cM_PER_DEGREE_EQ = 111318.163;

void PassThrough()
{
   for (int i = 0; i < 2; ++i)
   {
      vec4 p = gl_ProjectionMatrix * osg_ViewMatrix * gl_in[i].gl_Position;
      p.z = min(p.w, p.z);
      gl_Position = p;
      EmitVertex();
   }
   EndPrimitive();
}

void SplitProcessing()
{
   for (int i = 0; i < 2; ++i)
   {
      vec4 p = gl_in[i].gl_Position;
      vec4 q = gl_in[1 - i].gl_Position;
      float s = sign(gl_in[i].gl_Position.x);

      vec4 p1 = gl_ProjectionMatrix * osg_ViewMatrix * p;
      p1.z = min(p1.w, p1.z);
      gl_Position = p1;	  
      EmitVertex();

      q.x += s * cM_PER_DEGREE_EQ * 360.0;
      float t = (s * cM_PER_DEGREE_EQ * 180.0 - p.x) / (q.x - p.x);
      vec4 p2 = q * t + p * (1.0 - t);

      p2 = gl_ProjectionMatrix * osg_ViewMatrix * p2;
      p2.z = min(p2.w, p2.z);
      gl_Position = p2;
      EmitVertex();

      EndPrimitive();	  
   }
}

void main()
{
   if (sign(ecefCoord[0].x) < 0 && sign(ecefCoord[0].y) != sign(ecefCoord[1].y))
   {
      SplitProcessing();
   }
   else
   {
      PassThrough();
   }
}
