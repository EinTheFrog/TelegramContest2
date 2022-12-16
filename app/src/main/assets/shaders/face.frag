#extension GL_OES_EGL_image_external : require

precision mediump float;

uniform sampler2D u_texture;
uniform samplerExternalOES camTexture;

varying vec2 v_texCoord;
varying vec3 v_color;
varying vec2 v_adjustedCoord;

void main() {
    float alpha = texture2D(u_texture, vec2(v_texCoord.x, 1.0 - v_texCoord.y)).x;
    vec3 color = texture2D(camTexture, v_adjustedCoord).xyz;

    vec3 magneta = vec3(1.0, 0.0, 1.0);
    gl_FragColor.rgb = v_color;
    gl_FragColor.a = alpha;
}
