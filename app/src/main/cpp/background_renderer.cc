#include "background_renderer.h"

#include <type_traits>

namespace hello_ar {
namespace {
// Positions of the quad vertices in clip space (X, Y).
const GLfloat kVertices[] = {
    -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f,
};

constexpr char kCameraVertexShaderFilename[] = "shaders/screenquad.vert";
constexpr char kCameraFragmentShaderFilename[] = "shaders/screenquad.frag";

}  // namespace

void BackgroundRenderer::InitializeGlContent(AAssetManager* asset_manager) {
  glGenTextures(1, &camera_texture_id_);
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, camera_texture_id_);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  camera_program_ =
      util::CreateProgram(kCameraVertexShaderFilename,
                          kCameraFragmentShaderFilename, asset_manager);
  if (!camera_program_) {
    LOGE("Could not create program.");
  }

  camera_texture_uniform_ = glGetUniformLocation(camera_program_, "sTexture");
  camera_position_attrib_ = glGetAttribLocation(camera_program_, "a_Position");
  camera_tex_coord_attrib_ = glGetAttribLocation(camera_program_, "a_TexCoord");
}

void BackgroundRenderer::Draw(const ArSession* session, const ArFrame* frame) {
  static_assert(std::extent<decltype(kVertices)>::value == kNumVertices * 2,
                "Incorrect kVertices length");

  // If display rotation changed (also includes view size change), we need to
  // re-query the uv coordinates for the on-screen portion of the camera image.
  int32_t geometry_changed = 0;
  ArFrame_getDisplayGeometryChanged(session, frame, &geometry_changed);
/*  if (geometry_changed != 0 || !uvs_initialized_) {
    ArFrame_transformCoordinates2d(
        session, frame, AR_COORDINATES_2D_OPENGL_NORMALIZED_DEVICE_COORDINATES,
        kNumVertices, kVertices, AR_COORDINATES_2D_TEXTURE_NORMALIZED,
        transformed_uvs_);
    uvs_initialized_ = true;
  }*/

  int64_t frame_timestamp;
  ArFrame_getTimestamp(session, frame, &frame_timestamp);
  if (frame_timestamp == 0) {
    // Suppress rendering if the camera did not produce the first frame yet.
    // This is to avoid drawing possible leftover data from previous sessions if
    // the texture is reused.
    return;
  }

  if (camera_texture_id_ == -1) {
    return;
  }

  glDepthMask(GL_FALSE);

  glBindTexture(GL_TEXTURE_EXTERNAL_OES, camera_texture_id_);
  glUseProgram(camera_program_);
  glUniform1i(camera_texture_uniform_, 0);

  // Set the vertex positions and texture coordinates.
  glVertexAttribPointer(camera_position_attrib_, 2, GL_FLOAT, false, 0,
                        kVertices);
  glVertexAttribPointer(camera_tex_coord_attrib_, 2, GL_FLOAT, false, 0,
                        kVertices);
  glEnableVertexAttribArray(camera_position_attrib_);
  glEnableVertexAttribArray(camera_tex_coord_attrib_);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Disable vertex arrays
  glDisableVertexAttribArray(camera_position_attrib_);
  glDisableVertexAttribArray(camera_tex_coord_attrib_);

  glUseProgram(0);
  glDepthMask(GL_TRUE);
  util::CheckGlError("BackgroundRenderer::Draw() error");
}

GLuint BackgroundRenderer::GetTextureId() const { return camera_texture_id_; }

}  // namespace hello_ar
