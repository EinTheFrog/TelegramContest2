#ifndef TELEGRAMCONTEST2_FACE_RENDERER_H
#define TELEGRAMCONTEST2_FACE_RENDERER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include "arcore_c_api.h"
#include "glm.h"

namespace hello_ar {

// PlaneRenderer renders ARCore plane type.
    class FaceRenderer {
    public:
        FaceRenderer() = default;
        ~FaceRenderer() = default;

        // Sets up OpenGL state used by the plane renderer.  Must be called on the
        // OpenGL thread.
        void InitializeGlContent(AAssetManager* asset_manager);

        // Draws the provided plane.
        void Draw(const glm::mat4& projection_mat, const glm::mat4& view_mat,
                  const ArSession& ar_session, const ArAugmentedFace& ar_face,
                  const ArFrame& ar_frame);

        GLuint GetTextureId() const;

    private:
        void UpdateForFace(const ArSession& ar_session, const ArAugmentedFace& ar_face);

        const float* vertices_;
        int32_t ver_count_;
        const uint16_t* triangles_;
        int32_t tri_count_;
        glm::mat4 model_mat_ = glm::mat4(1.0f);
        const float* tex_coords_;
        int32_t tex_count_;

        GLuint alpha_texture_id_;
        GLuint camera_texture_id_;

        GLuint shader_program_;
        GLint attri_vertices_;
        GLint uniform_mvp_mat_;
        GLint uniform_alpha_texture_;
        GLint attri_texcoords_;
        GLint uniform_camera_texture_;
    };
}  // namespace hello_ar

#endif  // C_ARCORE_HELLOE_AR_PLANE_RENDERER_H_
