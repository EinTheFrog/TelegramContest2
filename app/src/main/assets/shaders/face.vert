#extension GL_OES_EGL_image_external : require

uniform mat4 u_modelViewProjection;
uniform samplerExternalOES camTexture;

attribute vec4 a_position;
attribute vec2 a_texCoord;

varying vec2 v_texCoord;
varying vec3 v_color;
varying vec2 v_adjustedCoord;

void main() {
    v_texCoord = a_texCoord;
    vec4 projPos = u_modelViewProjection * a_position;
    vec4 normProjPos = projPos / projPos.w;
    v_adjustedCoord = vec2(normProjPos.y + 1.0, -normProjPos.x + 1.0) / 2.0;
    v_color = texture2D(camTexture, v_adjustedCoord).xyz;
    gl_Position = projPos;
}
