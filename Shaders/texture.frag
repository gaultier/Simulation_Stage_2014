// Version du GLSL

#version 150 core


// Entree

in vec2 coordTexture;


// Uniform

uniform sampler2D mtexture;


// Sortie

out vec4 out_Color;


// Fonction main

void main()
{
    // Couleur du pixel

    // light = 2
    out_Color = 2 * texture(mtexture, coordTexture);
}
