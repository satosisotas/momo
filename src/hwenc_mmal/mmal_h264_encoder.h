/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#ifndef MMAL_H264_ENCODER_H_
#define MMAL_H264_ENCODER_H_

extern "C"
{
#include "bcm_host.h"
#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_format.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/vcos/vcos.h"
}

#include <chrono>
#include <memory>
#include <vector>
#include <list>

#include "api/video_codecs/video_encoder.h"
#include "rtc_base/critical_section.h"
#include "common_video/h264/h264_bitstream_parser.h"
#include "common_video/include/bitrate_adjuster.h"
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "modules/video_coding/utility/quality_scaler.h"
#include "rtc_base/event.h"

class ProcessThread;

class MMALH264Encoder : public webrtc::VideoEncoder
{
public:
  explicit MMALH264Encoder(const cricket::VideoCodec &codec);
  ~MMALH264Encoder() override;

  int32_t InitEncode(const webrtc::VideoCodec *codec_settings,
                     int32_t number_of_cores,
                     size_t max_payload_size) override;
  int32_t RegisterEncodeCompleteCallback(
      webrtc::EncodedImageCallback *callback) override;
  int32_t Release() override;
  int32_t Encode(const webrtc::VideoFrame &frame,
                 const std::vector<webrtc::VideoFrameType> *frame_types) override;
  void SetRates(const RateControlParameters &parameters) override;
  webrtc::VideoEncoder::EncoderInfo GetEncoderInfo() const override;

private:
  int32_t MMALConfigure();
  void MMALRelease();
  static void MMALInputCallbackFunction(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer);
  void MMALInputCallback(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer);
  static void MMALOutputCallbackFunction(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer);
  void MMALOutputCallback(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer);
  void SetBitrateBps(uint32_t bitrate_bps);
  int32_t SendFrame(unsigned char *buffer, size_t size);

  webrtc::EncodedImageCallback *callback_;
  MMAL_COMPONENT_T* encoder_;
  VCOS_SEMAPHORE_T semaphore_;
  MMAL_QUEUE_T *queue_;
  MMAL_POOL_T *pool_in_;
  MMAL_POOL_T *pool_out_;
  webrtc::BitrateAdjuster bitrate_adjuster_;
  uint32_t framerate_;
  int32_t configured_framerate_;
  uint32_t target_bitrate_bps_;
  uint32_t configured_bitrate_bps_;
  int32_t width_;
  int32_t height_;
  int32_t configured_width_;
  int32_t configured_height_;
  int32_t stride_width_;
  int32_t stride_height_;

  webrtc::H264BitstreamParser h264_bitstream_parser_;

  webrtc::EncodedImage encoded_image_;

  std::chrono::system_clock::time_point start_;
};

#endif // MMAL_H264_ENCODER_H_
