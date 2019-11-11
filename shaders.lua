
local ShaderProgram = require("wrap/ShaderProgram")

shaders = {}

shaders.flat = ShaderProgram:new {
    vertexAttributes = {
        "vertex",
    },

    uniforms = {
        transform = "matrix",
        projection = "matrix",
        color = "vector"
    },

    code = {
        vertex = 
[[
#version 330

in vec4 vertex;
in vec4 normal;

out vec4 fragNormal;

uniform mat4 transform;
uniform mat4 projection;

void main()
{
    gl_Position = projection * transform * vec4(vertex.xyz, 1);
    fragNormal = transform * vec4(normal.xyz, 0);
}
]],
        fragment =
[[
#version 330

in vec4 fragNormal;
out vec4 fragColor;

uniform vec4 color;

void main()
{
    fragColor = color;
}
]]
    }
}

shaders.default = ShaderProgram:new {
    vertexAttributes = {
        "vertex",
        "normal",
        "texCo",
        "boneWeights1",
        "boneWeights2",
        "boneWeights3",
        "boneWeights4",
        "boneWeights5",
        "boneWeights6",
        "boneWeights7",
        "boneWeights8"
    },

    uniforms = {
        transform = "matrix",
        projection = "matrix",
        diffuseColor = "vector",
        diffuseTexture = "texture2d",
        enableDiffuseTexture = "boolean",
        shadeless = "boolean",
        boneCount = "integer",
        boneMatrix = "matrix",
        lightCount = "integer",
        lightPositionAndDistance = "vector"
    },

    code = {
        vertex = 
[[
#version 330

in vec4 vertex;
in vec4 normal;
in vec4 texCo;

in vec4 boneWeights1;
in vec4 boneWeights2;
in vec4 boneWeights3;
in vec4 boneWeights4;
in vec4 boneWeights5;
in vec4 boneWeights6;
in vec4 boneWeights7;
in vec4 boneWeights8;

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 fragTexCo;

uniform mat4 transform;
uniform mat4 projection;

uniform int boneCount;
uniform mat4 boneMatrix[32];


float getBoneWeight(int index)
{
    if(index < 4)
        return boneWeights1[index];
    else if(index < 8)
        return boneWeights2[index - 4];
    else if(index < 12)
        return boneWeights3[index - 8];
    else if(index < 16)
        return boneWeights4[index - 12];
    else if(index < 20)
        return boneWeights5[index - 16];
    else if(index < 24)
        return boneWeights6[index - 20];
    else if(index < 28)
        return boneWeights7[index - 24];
    else if(index < 32)
        return boneWeights8[index - 28];
}

void main()
{
    vec3 vertexWithBones;
    vec3 normalWithBones;

    float totalWeight = 0.0;
    for(int i = 0; i < boneCount; i++)
    {
        totalWeight += getBoneWeight(i);
    }
    
    if(totalWeight > 0.0001)
    {
        vertexWithBones = vec3(0);
        normalWithBones = vec3(0);
        for(int i = 0; i < boneCount; i++)
        {
            vertexWithBones = vertexWithBones
                + (boneMatrix[i] * vec4(vertex.xyz, 1)).xyz
                    * getBoneWeight(i) / totalWeight;
            normalWithBones = normalWithBones
                + (boneMatrix[i] * vec4(normal.xyz, 0)).xyz
                    * getBoneWeight(i) / totalWeight;
        }
    }
    else
    {
        vertexWithBones = vec3(vertex.xyz);
        normalWithBones = vec3(normal.xyz);
    }

    fragTexCo = texCo.xy;
    fragPosition = (transform * vec4(vertexWithBones, 1)).xyz;
    gl_Position = projection * vec4(fragPosition, 1);
    fragNormal = normalize((transform * vec4(normalWithBones.xyz, 0)).xyz);
}
]],
        fragment =
[[
#version 330

uniform mat4 projection;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCo;

out vec4 fragColor;

uniform vec4 diffuseColor;
uniform sampler2D diffuseTexture;
uniform int enableDiffuseTexture;
uniform int shadeless;

uniform int lightCount;
uniform vec4 lightPositionAndDistance[32];

void main()
{
    vec3 color = diffuseColor.xyz;
    float alpha = diffuseColor.w;
    
    if(enableDiffuseTexture == 1)
    {
        vec4 sample = texture(diffuseTexture, fragTexCo.xy);
        color = sample.xyz;
        alpha = sample.w;
    }

    if(shadeless == 1)
        fragColor = vec4(color, alpha);
    else
    {
        float brightness = 0;
        vec3 l;
        for(int i = 0; i < lightCount; i ++)
        {
            l = lightPositionAndDistance[i].xyz - fragPosition;
            brightness += clamp(dot(normalize(l), fragNormal), 0, 1)
                * clamp(lightPositionAndDistance[i].w / dot(l, l), 0, 1);
        }
        fragColor = vec4(brightness * color, alpha);
    }
}
]]
    }
}

return shaders
