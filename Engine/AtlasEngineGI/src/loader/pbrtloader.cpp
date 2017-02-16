#include "..\..\include\loader\pbrtloader.h"

PBRTLoader::PBRTLoader() {
}

void PBRTLoader::loadFile(const std::string & pbrt_path, Scene * scene, MaterialLibrary & material_library) {
	std::string path = pbrt_path.substr(0, pbrt_path.find_last_of("/") + 1);
	std::vector<glm::vec3> positions;       //  Contiendra les positions de tous vertices
	std::vector<glm::vec3> normals;         //  Contiendra toutes les normales
	std::vector<glm::vec2> texture_coords;  //  Contiendra les coordonnées de texture de tous les vertices

	unsigned int indices_num = fillDataArrays(pbrt_path, positions, normals, texture_coords);

	addModels(material_library, scene, indices_num, positions, normals, texture_coords, pbrt_path, path);
}
