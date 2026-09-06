#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform float uFade;
uniform vec3 uColor;

void main(void)
{
   gl_FragColor = mix(vec4(1.0, 0.0, 0.0, 0.0), vec4(uColor, 1), uFade);
}
