#version 130
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

in float fieldValue;
out vec4 fragColor;

uniform sampler1D colorTable;
uniform float opacity;

void main()
{
   fragColor = texture(colorTable, fieldValue);
   fragColor.a = opacity;
}
