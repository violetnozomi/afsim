// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

precision mediump float;

flat in int oState;
uniform vec4 staticColor;
in float aTime;

uniform vec4 stateColors[16];

out vec4 outColor;

void main(void)
{
   outColor = (1.0 - staticColor.w) * stateColors[oState] + staticColor;
}
