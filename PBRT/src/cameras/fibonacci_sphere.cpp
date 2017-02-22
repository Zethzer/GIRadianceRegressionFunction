#include "fibonacci_sphere.h"

#include "paramset.h"

namespace pbrt {

FibonacciCamera::FibonacciCamera(const AnimatedTransform &CameraToWorld, Float shutterOpen,
              Float shutterClose, Film *film, const Medium *medium) :
    Camera(CameraToWorld, shutterOpen, shutterClose, film, medium)
{

}

Float FibonacciCamera::GenerateRay(const CameraSample &sample, Ray *ray) const {
    const Point2i res = this->film->fullResolution;
    const float PHI = 1.618f, PI = 3.141;

    ProfilePhase prof(Prof::GenerateCameraRay);

    //Direction calculation
    Float i = floor(sample.pFilm.y) * res.x + floor(sample.pFilm.x);
    Float n = res.y * res.x;

    Float p = 2.0f * PI * (i / PHI - floor(i / PHI));
    Float z = 1.0f - ((2.0f * i) + 1.0f) / n;
    z = std::min(std::max(z, -1.0f), 1.0f);
    Float t = acos(z);

    *ray = Ray(Point3f(0.0f, 0.0f, 0.0f), Vector3f(cos(p)*sin(t), sin(p)*sin(t), z));
    ray->time = Lerp(sample.time, shutterOpen, shutterClose);
    ray->medium = medium;
    *ray = CameraToWorld(*ray);

    if(isNaN(ray->d.x)) {
        std::cout << p << ";" << z << ";" << t << std::endl;
    }

    return 1.0f;
}

FibonacciCamera *CreateRandomCamera(const ParamSet &params,
                                  const AnimatedTransform &cam2world,
                                  Film *film, const Medium *medium) {

    Float shutteropen = params.FindOneFloat("shutteropen", 0.f);
    Float shutterclose = params.FindOneFloat("shutterclose", 1.f);

    if (shutterclose < shutteropen) {
        Warning("Shutter close time [%f] < shutter open [%f].  Swapping them.",
                shutterclose, shutteropen);
        std::swap(shutterclose, shutteropen);
    }

    return new FibonacciCamera(cam2world, shutteropen, shutterclose, film, medium);
}

}
