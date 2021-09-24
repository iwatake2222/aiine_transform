/* Copyright 2021 iwatake2222

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef IMAGE_PROCESSOR_H_
#define IMAGE_PROCESSOR_H_

/* for general */
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <array>

/* for OpenCV */
#include <opencv2/opencv.hpp>

/* for My modules */
#include "image_processor_if.h"
#include "face_detection_engine.h"
#include "facemesh_engine.h"

class ImageProcessor : public ImageProcessorIf {
private:
    typedef std::vector<std::array<cv::Point, 3>> Mesh3;

public:
    ImageProcessor();
    ~ImageProcessor() override;
    int32_t Initialize(const InputParam& input_param) override;
    int32_t Process(cv::Mat& image_result, cv::Mat& image_0, cv::Mat& image_1) override;
    int32_t Finalize(void) override;
    int32_t Command(int32_t cmd) override;

private:
    void DrawFps(const cv::Mat& image, cv::Point pos, double font_scale, int32_t thickness, cv::Scalar color_front, cv::Scalar color_back, bool is_text_on_rect = true);
    void InitializeMeshIndices(void);
    void InitializeMeshIndicesForMouth(void);
    void WarpTriangle(const cv::Mat& image_src, const cv::Mat& image_dst, const std::array<cv::Point, 3>& tri_src, const std::array<cv::Point, 3>& tri_dst);
    int32_t CreateMesh(cv::Mat& image, cv::Mat& image_face, std::vector<Mesh3>& mesh_list);
    void DrawMesh(cv::Mat& image, const std::vector<Mesh3>& mesh_list);
private:
    int32_t mode_;
    int32_t main_image_num_;
    int32_t frame_cnt_;
    FaceDetectionEngine facedet_engine_;
    FacemeshEngine facemesh_engine_;

    std::vector<int32_t> mesh_indices_;

    cv::Mat image_0_;
    cv::Mat image_1_;
    cv::Mat image_face_0_;
    cv::Mat image_face_1_;
    std::vector<Mesh3> mesh_list_0_;
    std::vector<Mesh3> mesh_list_1_;
};

#endif
