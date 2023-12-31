#version 330

in vec3 v_color;

out vec3 f_color;

void main() {
    vec2 pixel_coord = (gl_FragCoord.xy-256.0)/256.0;
    f_color = v_color*(1.0-length(pixel_coord));
}
