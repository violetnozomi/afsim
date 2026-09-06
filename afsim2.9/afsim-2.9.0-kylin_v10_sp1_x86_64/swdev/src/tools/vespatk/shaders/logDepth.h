// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************


uniform float VA_LogDepth;
uniform mat4 UTO_ProjectionMatrixInverse;

void vtkLogDepth(inout vec4 vertex, mat4 projectionMatrix)
{
   mat4 clip2view = UTO_ProjectionMatrixInverse;
   vec4 farPoint = clip2view * vec4(0.0, 0.0, 1.0, 1.0);
   float FAR = -farPoint.z / farPoint.w;
   float C = 10.0;
   if (VA_LogDepth > 0.5) vertex.z = log(C * vertex.z + 1.0) / log(C * FAR + 1.0) * vertex.w;
}
