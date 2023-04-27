#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aColor;
layout (location = 4) in float aUseTexture;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;
smooth out vec4 Color;
out float UseTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;


//Branch bending input:
uniform float fBranchPhase; 
uniform float fDetailPhase; 
uniform float fTime;
uniform float fSpeed;
uniform float fDetailFreq;
uniform float fEdgeAtten; 
uniform float fDetailAmp;
uniform float fBranchAtten;
uniform float fBranchAmp;

out float fnewBranchPhase; 

vec4 SmoothCurve( vec4 x ) {   return x * x *( 3.0 - 2.0 * x ); } 
vec4 TriangleWave( vec4 x ) {   return abs( fract( x + 0.5 ) * 2.0 - 1.0 ); } 
vec4 SmoothTriangleWave( vec4 x ) {   return SmoothCurve( TriangleWave( x ) ); } 

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;  
    Color = aColor;
    UseTexture = aUseTexture;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

   // Phases (object, vertex, branch)    
   
   float fObjPhase = dot(aPos, vec3(1, 1, 1)); 
   fnewBranchPhase = fBranchPhase+ fObjPhase; 
   float valueEdges = (fDetailPhase + fBranchPhase);
   float fVtxPhase = dot(aPos,  vec3(valueEdges, valueEdges, 1)); // x is used for edges; y is used for branches    
   vec2 vWavesIn = fTime + vec2(fVtxPhase, fBranchPhase ); // 1.975, 0.793, 0.375, 0.193 are good frequencies    
   vec4 vWaves = (fract( vec4(vWavesIn.x, vWavesIn.x, vWavesIn.y, vWavesIn.y) *vec4(1.975, 0.793, 0.375, 0.193) ) * 2.0 - 1.0 ) * fSpeed * fDetailFreq; 
   vWaves = SmoothTriangleWave( vWaves ); 
   vec2 vWavesSum = vWaves.xz + vWaves.yw; 
   // Edge (xy) and branch bending (z) 
   vec3 newPos = aPos + vWavesSum.xxy * vec3(fEdgeAtten * fDetailAmp * aNormal.xy, fBranchAtten * fBranchAmp);
  

    gl_Position = projection * view * model * vec4(newPos, 1.0);
}	