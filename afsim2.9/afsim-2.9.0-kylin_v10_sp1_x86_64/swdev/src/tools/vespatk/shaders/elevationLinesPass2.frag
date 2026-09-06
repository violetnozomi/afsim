// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform sampler2D texture;
uniform sampler1D spectrum;

uniform float colorScalar;
uniform vec2 colorSpace;

float GetPixel(int x, int y)
{
   return textureOffset(texture, gl_TexCoord[0].xy, ivec2(x, y)).r;
}

float ComputeEdgeColor()
{
   float p00 = GetPixel(-1, -1);
   float p10 = GetPixel(0, -1);
   float p20 = GetPixel(1, -1);
   float p01 = GetPixel(-1, 0);
   float p21 = GetPixel(1, 0);
   float p02 = GetPixel(-1, 1);
   float p12 = GetPixel(0, 1);
   float p22 = GetPixel(1, 1);

   float horiz = 3.0 * p00 - 3.0 * p20 + 8.0 * p01 - 8.0 * p21 + 3.0 * p02 - 3.0 * p22;
   float vert = 3.0 * p00 + 8.0 * p10 + 3.0 * p20 - 3.0 * p02 - 8.0 * p12 - 3.0 * p22;

   return sqrt(horiz * horiz + vert * vert);
}

void main()
{
   float height = (GetPixel(0, 0) / colorScalar - colorSpace.x) / (colorSpace.y - colorSpace.x); // get a normalized height (min->0, max->1)
   gl_FragColor = vec4(texture1D(spectrum, height) * vec4(1.0, 1.0, 1.0, ComputeEdgeColor()));
}
