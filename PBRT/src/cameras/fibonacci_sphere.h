#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_CAMERAS_RANDOM_H
#define PBRT_CAMERAS_RANDOM_H

// cameras/fibonacci_sphere.h*
#include "pbrt.h"
#include "camera.h"
#include "film.h"

namespace pbrt {

class FibonacciCamera : public Camera {
public:
    FibonacciCamera(const AnimatedTransform &CameraToWorld, Float shutterOpen,
                  Float shutterClose, Film *film, const Medium *medium);
    virtual ~FibonacciCamera() { }

    Float GenerateRay(const CameraSample &sample, Ray *ray) const;

private:
};

FibonacciCamera *CreateRandomCamera(const ParamSet &params,
                                  const AnimatedTransform &cam2world,
                                  Film *film, const Medium *medium);

}

#endif//PBRT_CAMERAS_RANDOM_H
