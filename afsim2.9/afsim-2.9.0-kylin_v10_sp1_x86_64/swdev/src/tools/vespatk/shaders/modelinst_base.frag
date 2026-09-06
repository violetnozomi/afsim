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
uniform float VA_SuppressTeamColor;
uniform vec4 uto_LightSourceAmbient;
uniform vec4 uto_LightSourceDiffuse;
uniform vec4 uto_LightSourceSpecular;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_emission;

in vec3 lightVec;
in vec3 eyeVec;
in vec4 teamColor;
in vec2 texCoord;

out vec4 outColor;

void main(void)
{
   vec4 vColor = texture(texture0, texCoord);
   // we need an ambient material color, lets assume 1.0, since I can't rely on the model
   vec4 vAmbient = uto_LightSourceAmbient * material_ambient;
   vec4 vEmission = material_emission;
   float diffuse = max(lightVec.z, 0.0);
   // lets just assume the diffuse value is 1.0
   vec4 vDiffuse = uto_LightSourceDiffuse * diffuse * material_diffuse;
   float specular = pow(clamp(dot(reflect(lightVec, vec3(0.0, 1.0, 0.0)), eyeVec), 0.0, 1.0), 10.0);
   vec4 vSpecular = uto_LightSourceSpecular * specular * diffuse;
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

   outColor = vec4(litAndTeamColor.rgb, vColor.a * teamColor.a);
}
