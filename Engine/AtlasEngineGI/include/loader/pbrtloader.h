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
	void addModels(MaterialLibrary &material_library, Scene *scene, const std::string &pbrt_path, const std::string &path);
	void buildMeshData(std::vector<Vertex> &vertices, const std::vector<unsigned int> indices, std::vector<glm::vec3> positions, 
		const std::vector<glm::vec3> normals, const std::vector<glm::vec2> texture_coords);
	std::string extractNextParameter(std::string &line, std::string &valuesString, bool *values, int *pos);
	std::string extractValues(std::string &line);
	std::vector<unsigned int> parseIntegerValues(std::string &values);
	std::vector<float> parseFloatValues(std::string &values);
	glm::vec3 parseVec3Values(std::string &values);
	std::vector<glm::vec3> parseManyVec3Values(std::string &values);
	std::vector<glm::vec2> parseManyVec2Values(std::string &values);

	// For debug
	void displayVec3(glm::vec3 t);
	void displayVec2(glm::vec2 t);
	int countVertex;
};

#endif // PBRTLOADER_H
