#ifndef PBRTLOADER_H
#define PBRTLOADER_H

#include <string>
#include <vector>

#include "mesh.h"

class Material;
class Scene;
struct MaterialLibrary;

class PBRTLoader
{
public:
	PBRTLoader();

	void loadFile(const std::string &pbrt_path, Scene *scene, MaterialLibrary &material_library);
};

#endif // PBRTLOADER_H
