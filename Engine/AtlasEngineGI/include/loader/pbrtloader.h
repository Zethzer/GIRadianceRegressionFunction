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

private:
	void fillDataArrays(const std::string &pbrt_path, std::vector<glm::vec3> &position, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &texture_coords);
	void addModels(MaterialLibrary &material_library, Scene *scene, const std::vector<glm::vec3> positions, const std::vector<glm::vec3> normals, 
		const std::vector<glm::vec2> texture_coords, const std::string &pbrt_path, const std::string &path) const;
	std::string extractNextParameter(std::string &line, std::string &valuesString, bool *values, int *pos) const;
	std::string extractValues(std::string &line) const;
};

#endif // PBRTLOADER_H
