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
/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <chrono>

/* for OpenCV */
#include <opencv2/opencv.hpp>

/* for My modules */
#include "image_processor.h"
#include "common_helper_cv.h"

/*** Macro ***/
#define WORK_DIR RESOURCE_DIR
static constexpr char kDefaultInputImage[] = RESOURCE_DIR"/face_06.jpg";
static constexpr char kDefaultMaskImage[] = RESOURCE_DIR"/bakatono.jpg";
static constexpr int32_t kLoopNumForTimeMeasurement = 10;
static constexpr int32_t kNumThread = 4;

/*** Global variable ***/
static bool is_pause = false;
static bool is_process_one_frame = false;

/*** Function ***/
static void TreatKeyInputMain(std::unique_ptr<ImageProcessorIf>& image_processor, int32_t key, cv::VideoCapture& cap, cv::Mat& image_face)
{
    is_process_one_frame = false;

    key &= 0xFF;
    if ('0' <= key && key <= '9') {
        image_processor->Command(key - '0');
    } else {
        do {
            switch (key) {
            case 'f':
                cap.read(image_face);
                break;
            case 'g':
                image_face = cv::imread(kDefaultMaskImage);
                break;
            case 'p':
                is_pause = !is_pause;
                break;
            case '>':
                if (is_pause) {
                    is_process_one_frame = true;
                } else {
                    int32_t current_frame = static_cast<int32_t>(cap.get(cv::CAP_PROP_POS_FRAMES));
                    cap.set(cv::CAP_PROP_POS_FRAMES, current_frame + 100);
                }
                break;
            case '<':
                int32_t current_frame = static_cast<int32_t>(cap.get(cv::CAP_PROP_POS_FRAMES));
                if (is_pause) {
                    is_process_one_frame = true;
                    cap.set(cv::CAP_PROP_POS_FRAMES, current_frame - 2);
                } else {
                    cap.set(cv::CAP_PROP_POS_FRAMES, current_frame - 100);
                }
                break;
            }
            if (is_pause) key = cv::waitKey(1) & 0xFF;
        } while (is_pause && !is_process_one_frame);
    }
}


int32_t main(int argc, char* argv[])
{
    /*** Initialize ***/
    /* variables for processing time measurement */
    double total_time_all = 0;

    /* Find source image */
    std::string input_name = (argc > 1) ? argv[1] : kDefaultInputImage;
    cv::VideoCapture cap;   /* if cap is not opened, src is still image */
    if (!CommonHelper::FindSourceImage(input_name, cap)) {
        return -1;
    }

    /* Create video writer to save output video */
    cv::VideoWriter writer;
    // writer = cv::VideoWriter("out.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'), (std::max)(10.0, cap.get(cv::CAP_PROP_FPS)), cv::Size(static_cast<int32_t>(cap.get(cv::CAP_PROP_FRAME_WIDTH)), static_cast<int32_t>(cap.get(cv::CAP_PROP_FRAME_HEIGHT))));

    /* Initialize image processor library */
    cv::setNumThreads(kNumThread);
    std::unique_ptr<ImageProcessorIf> image_processor = ImageProcessorIf::Create();
    ImageProcessorIf::InputParam input_param = { RESOURCE_DIR, kNumThread };
    if (image_processor->Initialize(input_param) != 0) {
        printf("Initialization Error\n");
        return -1;
    }

    cv::Mat image_face;
    image_face = cv::imread(kDefaultMaskImage);

    /*** Process for each frame ***/
    int32_t frame_cnt = 0;
    for (frame_cnt = 0; cap.isOpened() || frame_cnt < kLoopNumForTimeMeasurement; frame_cnt++) {
        const auto& time_all0 = std::chrono::steady_clock::now();
        /* Read image */
        cv::Mat image;
        if (cap.isOpened()) {
            cap.read(image);
        } else {
            image = cv::imread(input_name);
        }
        if (image.empty()) break;

        /* Call image processor library */
        cv::Mat image_result;
        image_processor->Process(image_result, image, image_face);
        image_face.release();

        /* Display result */
        if (writer.isOpened()) writer.write(image_result);
        cv::imshow("test", image_result);

        /* Input key command */
        if (cap.isOpened()) {
            int32_t key = cv::waitKey(1);
            if (key == 27) break;   /* ESC to quit */
            TreatKeyInputMain(image_processor, key, cap, image_face);
        };

        /* Print processing time */
        const auto& time_all1 = std::chrono::steady_clock::now();
        double time_all = (time_all1 - time_all0).count() / 1000000.0;
        printf("Total:               %9.3lf [msec]\n", time_all);
        printf("=== Finished %d frame ===\n\n", frame_cnt);

        if (frame_cnt > 0) {    /* do not count the first process because it may include initialize process */
            total_time_all += time_all;
        }
    }
    
    /*** Finalize ***/
    /* Print average processing time */
    if (frame_cnt > 1) {
        frame_cnt--;    /* because the first process was not counted */
        printf("=== Average processing time ===\n");
        printf("Total:               %9.3lf [msec]\n", total_time_all / frame_cnt);
    }

    /* Fianlize image processor library */
    image_processor->Finalize();
    if (writer.isOpened()) writer.release();
    cv::waitKey(-1);

    return 0;
}
