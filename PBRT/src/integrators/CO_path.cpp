 // integrators/CO_path.cpp*
#include "integrators/CO_path.h"
#include "bssrdf.h"
#include "camera.h"
#include "film.h"
#include "interaction.h"
#include "progressreporter.h"
#include "paramset.h"
#include "scene.h"
#include "stats.h"

namespace pbrt {

STAT_COUNTER("Integrator/Camera rays traced", nCameraRays);
STAT_PERCENT("Integrator/Zero-radiance paths", zeroRadiancePaths, totalPaths);
STAT_INT_DISTRIBUTION("Integrator/Path length", pathLength);

// PathIntegrator Method Definitions
COPathIntegrator::COPathIntegrator(int maxDepth,
                               std::shared_ptr<const Camera> camera,
                               std::shared_ptr<Sampler> sampler,
                               const Bounds2i &pixelBounds, Float rrThreshold,
                               const std::string &lightSampleStrategy,
                               const float maxVariance)
    : PathIntegrator(maxDepth, camera, sampler, pixelBounds, rrThreshold, lightSampleStrategy, true),
      maxVariance_(maxVariance)
{

}


// SamplerIntegrator Method Definitions
void COPathIntegrator::Render(const Scene &scene) {
    Preprocess(scene, *sampler);
    // Render image tiles in parallel

    // Compute number of tiles, _nTiles_, to use for parallel rendering
    Bounds2i sampleBounds = camera->film->GetSampleBounds();
    Vector2i sampleExtent = sampleBounds.Diagonal();
    const int tileSize = 16;
    Point2i nTiles((sampleExtent.x + tileSize - 1) / tileSize,
                   (sampleExtent.y + tileSize - 1) / tileSize);
    ProgressReporter reporter(nTiles.x * nTiles.y, "Rendering");
    {
        ParallelFor2D([&](Point2i tile) {
            // Render section of image corresponding to _tile_

            // Allocate _MemoryArena_ for tile
            MemoryArena arena;

            // Get sampler instance for tile
            int seed = tile.y * nTiles.x + tile.x;
            std::unique_ptr<Sampler> tileSampler = sampler->Clone(seed);

            // Compute sample bounds for tile
            int x0 = sampleBounds.pMin.x + tile.x * tileSize;
            int x1 = std::min(x0 + tileSize, sampleBounds.pMax.x);
            int y0 = sampleBounds.pMin.y + tile.y * tileSize;
            int y1 = std::min(y0 + tileSize, sampleBounds.pMax.y);
            Bounds2i tileBounds(Point2i(x0, y0), Point2i(x1, y1));
            LOG(INFO) << "Starting image tile " << tileBounds;

            // Get _FilmTile_ for tile
            std::unique_ptr<FilmTile> filmTile =
                camera->film->GetFilmTile(tileBounds);

            // Loop over pixels in tile to render them
            for (Point2i pixel : tileBounds) {
                {
                    ProfilePhase pp(Prof::StartPixel);
                    tileSampler->StartPixel(pixel);
                }

                // Do this check after the StartPixel() call; this keeps
                // the usage of RNG values from (most) Samplers that use
                // RNGs consistent, which improves reproducability /
                // debugging.
                if (!InsideExclusive(pixel, pixelBounds))
                    continue;

                // Initialize _CameraSample_ for current sample
                CameraSample cameraSample =
                        tileSampler->GetCameraSample(pixel);

                // Generate camera ray for current sample
                RayDifferential ray;
                Float rayWeight = camera->GenerateRayDifferential(cameraSample, &ray);
                ++nCameraRays;

                LiResp R, Ri;

                Ri.Li = Spectrum(0.f);
                for(int i = 0; i < tileSampler->samplesPerPixel; ++i) {

                    // Evaluate radiance along new ray
                    if (rayWeight > 0) R = PathIntegrator::Li(ray, scene, *tileSampler, arena, 0);

                    // Issue warning if unexpected radiance value returned
                    if (R.Li.HasNaNs()) {
                        LOG(ERROR) << StringPrintf(
                                          "Not-a-number radiance value returned "
                                          "for pixel (%d, %d), sample %d. Setting to black.",
                                          pixel.x, pixel.y,
                                          (int)tileSampler->CurrentSampleNumber());
                        R.Li = Spectrum(0.f);
                    } else if (R.Li.y() < -1e-5) {
                        LOG(ERROR) << StringPrintf(
                                          "Negative luminance value, %f, returned "
                                          "for pixel (%d, %d), sample %d. Setting to black.",
                                          R.Li.y(), pixel.x, pixel.y,
                                          (int)tileSampler->CurrentSampleNumber());
                        R.Li = Spectrum(0.f);
                    } else if (std::isinf(R.Li.y())) {
                        LOG(ERROR) << StringPrintf(
                                          "Infinite luminance value returned "
                                          "for pixel (%d, %d), sample %d. Setting to black.",
                                          pixel.x, pixel.y,
                                          (int)tileSampler->CurrentSampleNumber());
                        R.Li = Spectrum(0.f);
                    }

                    VLOG(1) << "Intersection sample: " << cameraSample << " -> ray: " <<
                               ray << " -> L = " << R.Li;

                    // Add camera ray's contribution to image
                    Ri.Li += R.Li / tileSampler->samplesPerPixel;
                    Ri.pos = R.pos;
                    Ri.norm = R.norm;

                    // Free _MemoryArena_ memory from computing image sample
                    // value
                    arena.Reset();
                    tileSampler->StartNextSample();
                }
                filmTile->AddSample(cameraSample.pFilm, Ri.Li, 1.0f, Ri.pos, Ri.norm);
            }
            LOG(INFO) << "Finished image tile " << tileBounds;

            // Merge image tile into _Film_
            camera->film->MergeFilmTile(std::move(filmTile));
            reporter.Update();
        }, nTiles);
        reporter.Done();
    }
    LOG(INFO) << "Rendering finished";

    // Save final image after rendering
    camera->film->WriteImage();
}

COPathIntegrator *CreateCOPathIntegrator(const ParamSet &params,
                                     std::shared_ptr<Sampler> sampler,
                                     std::shared_ptr<const Camera> camera) {
    int maxDepth = params.FindOneInt("maxdepth", 5);
    float varMax = params.FindOneFloat("maxVariance", 0.5);
    int np;
    const int *pb = params.FindInt("pixelbounds", &np);
    Bounds2i pixelBounds = camera->film->GetSampleBounds();
    if (pb) {
        if (np != 4)
            Error("Expected four values for \"pixelbounds\" parameter. Got %d.",
                  np);
        else {
            pixelBounds = Intersect(pixelBounds,
                                    Bounds2i{{pb[0], pb[2]}, {pb[1], pb[3]}});
            if (pixelBounds.Area() == 0)
                Error("Degenerate \"pixelbounds\" specified.");
        }
    }
    Float rrThreshold = params.FindOneFloat("rrthreshold", 1.);
    std::string lightStrategy =
        params.FindOneString("lightsamplestrategy", "spatial");
    return new COPathIntegrator(maxDepth, camera, sampler, pixelBounds,
                              rrThreshold, lightStrategy);
}

}  // namespace pbrt
