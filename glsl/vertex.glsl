#version 330 core
in vec2 position;
in vec3 color;

uniform float width;
uniform float height;
out vec3 fragmentColor;

void main() {
    float x_pos = (float(position.x) / (width / 2.0)) - 1.0;
    float y_pos = (float(position.y) / (height / 2.0)) - 1.0;

    gl_Position = vec4(x_pos, y_pos, 0.0, 1.0);
    fragmentColor = color;
}
