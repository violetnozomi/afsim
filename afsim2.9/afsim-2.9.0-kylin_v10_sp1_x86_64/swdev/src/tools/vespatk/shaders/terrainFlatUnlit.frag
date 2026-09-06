#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// unused and untested at this version
uniform sampler2D Texture0;
uniform sampler2D Texture7;
uniform float fBrightener;
uniform float fDarkener;
uniform float fSaturation;
uniform float fProjAlpha;
uniform float fOpacity;

void main( void )
{
   vec4 vColor = texture2D(Texture0, gl_TexCoord[0].xy);
   vec4 vColor7 = texture2D(Texture7, gl_TexCoord[7].xy);
   float mean = 0.3333 * (vColor.r + vColor.g + vColor.b);
   vec4 vSatColor = vec4(fSaturation * (vColor.rgb - mean) + vColor.rgb, vColor.a);
   vec4 vBrightColor = vec4(vSatColor.rgb + (1.0 - vSatColor.rgb) * fBrightener + (-vSatColor.rgb) * fDarkener, vSatColor.a);
   float alpha = vColor7.a * fProjAlpha;
   gl_FragColor = (vColor7 * alpha + vBrightColor * (1.0 - alpha)) * vec4(1.0, 1.0, 1.0, fOpacity);
}
