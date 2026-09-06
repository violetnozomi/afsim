// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

precision mediump float;
uniform sampler2D texture0;
uniform vec4 color;
in float depth;
in vec2 texCoord;

out vec4 outColor;

void main(void)
{
   if (depth > 1.0)
   {
      discard;
   }
   vec4 tcolor = texture2D(texture0, texCoord);
   if (tcolor.a == 0.0)
   {
      discard;
   }
   outColor = tcolor * color;
}
