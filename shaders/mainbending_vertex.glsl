#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aColor;
layout (location = 4) in float aUseTexture;
layout(location = 5) in vec3 tangent;
layout(location = 6) in vec3 bitangent;
layout (location = 7) in ivec4 boneIds; 
layout (location = 8) in vec4 weights;

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

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

uniform vec4 vWind; 
uniform float fBendScale;
uniform float fLength; 

uniform bool useWeights;
uniform bool isGrass = false;



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


uniform float  branchSwayPowerA;
uniform float  branchSwayPowerB; 
uniform float  branchMovementRandomization;
uniform float  time; 
uniform float  timeWithDelay;
uniform float  branchSuppressPower;
uniform float  delayedWindPower;

out float fnewBranchPhase; 

vec4 SmoothCurve( vec4 x ) {   return x * x *( 3.0 - 2.0 * x ); } 
vec4 TriangleWave( vec4 x ) {   return abs( fract( x + 0.5 ) * 2.0 - 1.0 ); } 
vec4 SmoothTriangleWave( vec4 x ) {   return SmoothCurve( TriangleWave( x ) ); }

vec3 detailBending(vec4 totalPosition){

  // Phases (object, vertex, branch)    
   
   float fObjPhase = dot(totalPosition.xyz, vec3(1, 1, 1)); 
   fnewBranchPhase = fBranchPhase+ fObjPhase; 
   float valueEdges = (fDetailPhase + fBranchPhase);
   float fVtxPhase = dot(totalPosition.xyz,  vec3(valueEdges, valueEdges, 1)); // x is used for edges; y is used for branches    
   vec2 vWavesIn = fTime + vec2(fVtxPhase, fBranchPhase ); // 1.975, 0.793, 0.375, 0.193 are good frequencies    
   vec4 vWaves = (fract( vec4(vWavesIn.x, vWavesIn.x, vWavesIn.y, vWavesIn.y) *vec4(1.975, 0.793, 0.375, 0.193) ) * 2.0 - 1.0 ) * fSpeed * fDetailFreq; 
   vWaves = SmoothTriangleWave( vWaves ); 
   vec2 vWavesSum = vWaves.xz + vWaves.yw; 
   // Edge (xy) and branch bending (z) 
   vec3 newPos = totalPosition.xyz + vWavesSum.xxy * vec3(fEdgeAtten * fDetailAmp * aNormal.xy, fBranchAtten * fBranchAmp);

   return newPos; 
}

vec3 mainBending(vec4 totalPosition){
    // Bend factor - based on CPU wind variation    
      
      float fBF = abs(totalPosition.z) * fBendScale; 
   
    //Calculate smooth bending factor:
      fBF += 1.0; 
      fBF *= fBF; 
      fBF = fBF * fBF - fBF; 

    //Displace positions:    
      
      vec4 vNewPos = totalPosition; 
      vNewPos.xz += vWind.xz *fBF; 

     return vNewPos.xyz; 
}


vec3 grassBending(vec4 totalPosition){
    // Bend factor - based on CPU wind variation    
      
      float fBF = abs(totalPosition.y) * fBendScale; 
   
    //Calculate smooth bending factor:
      fBF += 1.0; 
      fBF *= fBF; 
      fBF = fBF * fBF - fBF; 

    //Displace positions:    
      
      vec4 vNewPos = totalPosition; 
      vNewPos.xz += vWind.xz *fBF; 

      //vNewPos.xyz = normalize(vNewPos.xyz)*5.0f;

     return vNewPos.xyz; 
}



void main(){
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;  
    Color = aColor;
    UseTexture = aUseTexture;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    vec4 totalPosition = vec4(0.0f);
  
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1){
             continue;
        } 
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(aPos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos,1.0f);


        totalPosition += localPosition * weights[i];

        //totalPosition = bendBranchLocal(totalPosition.xyz, boneIds[i], vWind.xyz, fBendScale) * totalPosition; 
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
   }

    //gl_Position = projection * view * model * vec4(aPos, 1.0);

     

     vec3 newPos =totalPosition.xyz;

     

     if(useWeights){
        if(isGrass){
            newPos = grassBending(vec4(newPos, 1));
         }
         else{
            newPos = mainBending(vec4(newPos, 1));
         }
     }
     else{
         newPos = mainBending(vec4(aPos, 1));
     }

     

    gl_Position = projection * view * model * vec4(newPos, 1);
}	