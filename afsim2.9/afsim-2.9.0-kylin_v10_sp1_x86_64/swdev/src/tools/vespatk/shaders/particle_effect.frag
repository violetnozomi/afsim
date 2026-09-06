// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform sampler2D texSampler;
varying vec2 texCoord;
varying vec4 effectColor;

void main( void )
{
   gl_FragColor = vec4(texture2D(texSampler, texCoord) * effectColor);
}
