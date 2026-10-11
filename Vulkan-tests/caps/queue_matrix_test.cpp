#include "vk_probe.hpp"
#include "vk_video_compat.hpp"

#include <vulkan/vulkan.h>

#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace canvas::vktest;

namespace {

constexpr std::uint32_t kDecodeOps = kAllDecodeOps;
constexpr std::uint32_t kEncodeOps = kAllEncodeOps;

int g_failures = 0;

void check(bool ok, const char* what) {
    std::printf("%s  %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) ++g_failures;
}

const char* decode_name(std::uint32_t op) {
    if (op == VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR) return "h264-decode";
    if (op == VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR) return "h265-decode";
    if (kDecodeAv1Op != 0 && op == kDecodeAv1Op) return "av1-decode";
    if (kDecodeVp9Op != 0 && op == kDecodeVp9Op) return "vp9-decode";
    return "?";
}
const char* encode_name(std::uint32_t op) {
    if (op == VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR) return "h264-encode";
    if (op == VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR) return "h265-encode";
    if (kEncodeAv1Op != 0 && op == kEncodeAv1Op) return "av1-encode";
    return "?";
}

}

int main() {
    const ProbeResult r = run_probe();
    if (!r.ok || r.devices.empty()) {
        std::printf("SKIP  queue_matrix: no Vulkan implementation (%s)\n",
                    r.ok ? "no devices" : r.error.c_str());
        return 2;
    }

    for (const auto& d : r.devices) {
        std::printf("queue_matrix: %s (%u families)\n", d.name.c_str(),
                    static_cast<unsigned>(d.queue_families.size()));
        for (const auto& q : d.queue_families) {
            std::printf("  family %u flags=0x%x dec=%s enc=%s gfx=%s cmp=%s\n",
                        q.index, q.flags,
                        q.video_decode ? "yes" : "no",
                        q.video_encode ? "yes" : "no",
                        q.graphics ? "yes" : "no",
                        q.compute ? "yes" : "no");
            const std::uint32_t kDecodeList[] = {
                VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR,
                VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR,
                kDecodeAv1Op,
                kDecodeVp9Op,
            };
            for (std::uint32_t op : kDecodeList) {
                if (op == 0) continue;
                if ((q.video_codec_operations & op) != 0)
                    std::printf("      %s\n", decode_name(op));
            }
            const std::uint32_t kEncodeList[] = {
                VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR,
                VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR,
                kEncodeAv1Op,
            };
            for (std::uint32_t op : kEncodeList) {
                if (op == 0) continue;
                if ((q.video_codec_operations & op) != 0)
                    std::printf("      %s\n", encode_name(op));
            }
        }

        bool coherent = true;
        for (const auto& q : d.queue_families) {
            const bool video_flag =
                (q.flags & (VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR)) != 0;
            if ((q.video_decode || q.video_encode) && !video_flag) coherent = false;
            if ((q.video_codec_operations & (kDecodeOps | kEncodeOps)) != 0 && !video_flag)
                coherent = false;
        }
        check(coherent, "queue_matrix: video ops only on video-flagged families");
    }

    const auto& d = r.devices.front();
    bool any_decode = false, any_encode = false, any_compute = false;
    for (const auto& q : d.queue_families) {
        any_decode |= q.video_decode;
        any_encode |= q.video_encode;
        any_compute |= q.compute;
    }
    check(any_compute, "queue_matrix: compute family present for composite/P-D");
    check(any_decode, "queue_matrix: decode family present (gates P-C readings)");
    check(any_encode, "queue_matrix: encode family present (gates P-E readings)");

    if (g_failures == 0) {
        std::printf("ALL TESTS PASSED\n");
        return 0;
    }
    std::printf("%d FAILURE(S)\n", g_failures);
    return 1;
}