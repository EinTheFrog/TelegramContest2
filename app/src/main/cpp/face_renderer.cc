/*
 * Copyright 2017 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "face_renderer.h"
#include <string>
#include "util.h"

namespace hello_ar {
    void FaceRenderer::InitializeGlContent(AAssetManager* asset_manager) {
        shader_program_ = util::CreateProgram("shaders/face.vert",
                                              "shaders/face.frag", asset_manager);
        if (!shader_program_) {
            LOGE("Could not create program.");
        }

        uniform_mvp_mat_ = glGetUniformLocation(shader_program_, "u_modelViewProjection");
        uniform_alpha_texture_ = glGetUniformLocation(shader_program_, "u_texture");
        attri_vertices_ = glGetAttribLocation(shader_program_, "a_position");
        attri_texcoords_ = glGetAttribLocation(shader_program_, "a_texCoord");
        uniform_camera_texture_ = glGetUniformLocation(shader_program_, "camTexture");

        glGenTextures(1, &alpha_texture_id_);
        glBindTexture(GL_TEXTURE_2D, alpha_texture_id_);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (!util::LoadPngFromAssetManager(GL_TEXTURE_2D, "models/mask.png")) {
            LOGE("Could not load png texture for planes.");
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &camera_texture_id_);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, camera_texture_id_);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        util::CheckGlError("face_renderer::InitializeGlContent()");
    }

    void FaceRenderer::Draw(const glm::mat4& projection_mat,
                             const glm::mat4& view_mat, const ArSession& ar_session,
                             const ArAugmentedFace& ar_face, const ArFrame& ar_frame) {
        if (!shader_program_) {
            LOGE("shader_program is null.");
            return;
        }

        int64_t frame_timestamp;
        ArFrame_getTimestamp(&ar_session, &ar_frame, &frame_timestamp);
        if (frame_timestamp == 0) {
            // Suppress rendering if the camera did not produce the first frame yet.
            // This is to avoid drawing possible leftover data from previous sessions if
            // the texture is reused.
            return;
        }

        UpdateForFace(ar_session, ar_face);

        glUseProgram(shader_program_);
        glDepthMask(GL_FALSE);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(uniform_alpha_texture_, 0);
        glBindTexture(GL_TEXTURE_2D, alpha_texture_id_);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, camera_texture_id_);
        glUniform1i(uniform_camera_texture_, 1);

        // Compose final mvp matrix for this face renderer.
        ArPose* center = nullptr;
        ArPose_create(&ar_session, NULL, &center);
        ArAugmentedFace_getCenterPose(&ar_session, &ar_face, center);
        float* out_mat = new float[16];
        ArPose_getMatrix(&ar_session, center, out_mat);
        model_mat_ = glm::make_mat4x4(out_mat);
        delete[] out_mat;
        glUniformMatrix4fv(uniform_mvp_mat_, 1, GL_FALSE,
                           glm::value_ptr(projection_mat * view_mat * model_mat_));
        glEnableVertexAttribArray(attri_vertices_);
        float verts[8] = {-1.0, -1.0,  -1.0, 0.0,  1.0, 0.0,  1.0, -1.0};
        glVertexAttribPointer(attri_vertices_, 3, GL_FLOAT, GL_FALSE, 0, vertices_);

        glEnableVertexAttribArray(attri_texcoords_);
/*        float uvs[8];
        ArFrame_transformCoordinates2d(
                &ar_session, &ar_frame, AR_COORDINATES_2D_OPENGL_NORMALIZED_DEVICE_COORDINATES,
                4, verts, AR_COORDINATES_2D_TEXTURE_NORMALIZED,
                uvs);*/
        glVertexAttribPointer(attri_texcoords_, 2, GL_FLOAT, GL_FALSE, 0, tex_coords_);



        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        uint16_t tris[6] = {0, 1, 2, 2, 3, 0};
        glDrawElements(GL_TRIANGLES, tri_count_ * 3, GL_UNSIGNED_SHORT, triangles_);

        glDisable(GL_BLEND);
        glUseProgram(0);
        glDepthMask(GL_TRUE);
        util::CheckGlError("face_renderer::Draw()");
    }

    void FaceRenderer::UpdateForFace(const ArSession& ar_session,
                                       const ArAugmentedFace& ar_face) {
        ArAugmentedFace_getMeshVertices(&ar_session, &ar_face, &vertices_, &ver_count_);
        ArAugmentedFace_getMeshTriangleIndices(&ar_session, &ar_face, &triangles_, &tri_count_);
        ArAugmentedFace_getMeshTextureCoordinates(&ar_session, &ar_face, &tex_coords_, &tex_count_);
    }

    GLuint FaceRenderer::GetTextureId() const { return camera_texture_id_; }

}  // namespace hello_ar
