// Version du GLSL

#version 150 core

in vec2 coordTexture;

// Uniform

uniform sampler2D mtexture;

out vec4 out_Color;

// Fonction main

void main()
{
    // Couleur du pixel

    // light = 2
    out_Color = 2 * texture(mtexture, coordTexture);
}
