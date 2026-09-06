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
uniform sampler2D texture0;
uniform float VA_SuppressTeamColor;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_emission;

varying vec3 lightVec;
varying vec3 eyeVec;
varying vec4 teamColor;
varying vec2 texCoord;

#include "screenspace.h"

void main(void)
{
   vec4 vColor = texture2D(texture0, texCoord);
   vec4 vAmbient = gl_LightSource[0].ambient * material_ambient;
   vec4 vEmission = material_emission;
   float diffuse = max(-lightVec.z, 0.0);
   vec4 vDiffuse = gl_LightSource[0].diffuse * material_diffuse * diffuse;
   float specular = pow(clamp(dot(reflect(lightVec, vec3(0.0, 0.0, 1.0)), eyeVec), 0.0, 1.0), 10.0);
   vec4 vSpecular = gl_LightSource[0].specular * specular * diffuse;
   // this mix() blends the texture color with the lighting color, based
   // on the "lighting" uniform
   vec3 litColor = min(vAmbient.rgb * vColor.rgb +
                       vDiffuse.rgb * vColor.rgb +
                       vEmission.rgb * vColor.rgb +
                       0.5 * vSpecular.rgb,
                       vec3(1.0));
   // these mix() calls blend the team color with the lit color, based on
   // the "VA_SuppressTeamColor" uniform

   vec4 litAndTeamColor = mix( vec4(litColor, vColor.a),
                               teamColor,
                               0.5 * (1.0 - VA_SuppressTeamColor) );

   gl_FragColor = vec4(litAndTeamColor.rgb, vColor.a * teamColor.a);
}
