#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_INTEGRATORS_CO_PATH_H
#define PBRT_INTEGRATORS_CO_PATH_H

// integrators/CO_path.h*
#include "pbrt.h"
#include "integrators/path.h"
#include "lightdistrib.h"

namespace pbrt {

// PathIntegrator Declarations
class COPathIntegrator : public PathIntegrator {
  public:
    // PathIntegrator Public Methods
    COPathIntegrator(int maxDepth, std::shared_ptr<const Camera> camera,
                   std::shared_ptr<Sampler> sampler,
                   const Bounds2i &pixelBounds, Float rrThreshold = 1,
                   const std::string &lightSampleStrategy = "spatial",
                   const float maxVariance = 0.05f);

    void Render(const Scene &scene);

  private:
    float maxVariance_;
};

COPathIntegrator *CreateCOPathIntegrator(const ParamSet &params,
                                     std::shared_ptr<Sampler> sampler,
                                     std::shared_ptr<const Camera> camera);

}  // namespace pbrt

#endif  // PBRT_INTEGRATORS_PATH_H
