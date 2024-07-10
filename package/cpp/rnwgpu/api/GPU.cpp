#include "GPU.h"
#include <utility>

namespace rnwgpu {

std::future<std::shared_ptr<GPUAdapter>>
GPU::requestAdapter(std::shared_ptr<GPURequestAdapterOptions> options) {
  return std::async(std::launch::async, [this, options]() {
    auto aOptions = options->getInstance();
    wgpu::Adapter adapter = nullptr;
    _instance.RequestAdapter(
        aOptions,
        [](WGPURequestAdapterStatus, WGPUAdapter cAdapter, const char *message,
           void *userdata) {
          if (message != nullptr) {
            fprintf(stderr, "%s", message);
            return;
          }
          *static_cast<wgpu::Adapter *>(userdata) =
              wgpu::Adapter::Acquire(cAdapter);
        },
        &adapter);
    // TODO: implement returning null jsi value
    if (!adapter) {
      throw std::runtime_error("Failed to request adapter");
    }

    return std::make_shared<GPUAdapter>(std::move(adapter));
  });
}

wgpu::TextureFormat GPU::getPreferredCanvasFormat() {
#if defined(__ANDROID__)
  return wgpu::TextureFormat::RGBA8Unorm;
#else
  return wgpu::TextureFormat::BGRA8Unorm;
#endif // defined(__ANDROID__)
}

} // namespace rnwgpu
