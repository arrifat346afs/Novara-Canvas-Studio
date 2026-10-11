#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace canvas::vktest {

#ifdef VK_KHR_VIDEO_DECODE_AV1_EXTENSION_NAME
constexpr std::uint32_t kDecodeAv1Op = VK_VIDEO_CODEC_OPERATION_DECODE_AV1_BIT_KHR;
#else
constexpr std::uint32_t kDecodeAv1Op = 0;
#endif

#ifdef VK_KHR_VIDEO_DECODE_VP9_EXTENSION_NAME
constexpr std::uint32_t kDecodeVp9Op = VK_VIDEO_CODEC_OPERATION_DECODE_VP9_BIT_KHR;
#else
constexpr std::uint32_t kDecodeVp9Op = 0;
#endif

#ifdef VK_KHR_VIDEO_ENCODE_AV1_EXTENSION_NAME
constexpr std::uint32_t kEncodeAv1Op = VK_VIDEO_CODEC_OPERATION_ENCODE_AV1_BIT_KHR;
#else
constexpr std::uint32_t kEncodeAv1Op = 0;
#endif

constexpr std::uint32_t kDecodeBaseOps = VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR |
                                         VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR;
constexpr std::uint32_t kEncodeBaseOps = VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR |
                                         VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR;
constexpr std::uint32_t kAllDecodeOps = kDecodeBaseOps | kDecodeAv1Op | kDecodeVp9Op;
constexpr std::uint32_t kAllEncodeOps = kEncodeBaseOps | kEncodeAv1Op;

}
