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
uniform vec4 uto_LightSourceAmbient;
uniform vec4 uto_LightSourceDiffuse;
uniform vec4 uto_LightSourceSpecular;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_emission;

in vec2 oTexCoord;
in vec3 oNormal;
in vec3 oLight;
in vec3 oFragPos;
in vec3 oViewPos;

out vec4 outColor;

void main(void)
{
   vec4 vColor = texture(texture0, oTexCoord);

   vec4 ambientColor = material_ambient * uto_LightSourceAmbient;

   vec3 lightDir = normalize(oLight);
   vec3 norm = normalize(oNormal);
   vec4 diffuseColor = material_diffuse * uto_LightSourceDiffuse * vec4(vec3(dot(lightDir, norm)), 1.0);

   vec3 viewDir = -normalize(oViewPos - oFragPos);
   vec3 reflectDir = reflect(-oLight, norm);
   float specular = pow(max(dot(viewDir, reflectDir), 0.0), 10.0);
   vec4 specularColor = uto_LightSourceSpecular * specular;

   outColor = vColor * (ambientColor + diffuseColor + material_emission);// + vec4(specularColor.rgb, 0.0);
}
