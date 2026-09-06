// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform sampler2D texture0;
varying vec2 texCoord;
uniform vec4 staticColor;

void main(void)
{
   vec4 tex = texture2D(texture0, texCoord);
   gl_FragColor = tex + staticColor*tex.a;
}