#include "pbrtloader.h"
#include "material.h"
#include "model.h"
#include "scenegraphnode.h"
#include "scene.h"

PBRTLoader::PBRTLoader() {
}

void PBRTLoader::loadFile(const std::string & pbrt_path, Scene * scene, MaterialLibrary & material_library) {
	std::string path = pbrt_path.substr(0, pbrt_path.find_last_of("/") + 1);
	std::vector<glm::vec3> positions;       //  Contiendra les positions de tous vertices
	std::vector<glm::vec3> normals;         //  Contiendra toutes les normales
	std::vector<glm::vec2> texture_coords;  //  Contiendra les coordonnées de texture de tous les vertices

	fillDataArrays(pbrt_path, positions, normals, texture_coords);

	addModels(material_library, scene, positions, normals, texture_coords, pbrt_path, path);
}

void PBRTLoader::fillDataArrays(const std::string &pbrt_path, std::vector<glm::vec3> &position, std::vector<glm::vec3> &normals, 
	std::vector<glm::vec2> &texture_coords) {
	// to do
}

void PBRTLoader::addModels(MaterialLibrary &material_library, Scene *scene, const std::vector<glm::vec3> positions, 
	const std::vector<glm::vec3> normals, const std::vector<glm::vec2> texture_coords, const std::string &pbrt_path, const std::string &path) const {
	SceneGraphRoot *root = new SceneGraphRoot("root", path);
	Material *current_material = new Material();
	float color1;
	float color2;
	float color3;
	std::ifstream pbrt_file;

	pbrt_file.open(pbrt_path.c_str());

	if (!(pbrt_file)) {
		std::cerr << "PBRTLOADER::ADDMODELS::ERROR Probleme lors de la lecture du fichier .pbrt" << std::endl;
		exit(1);
	}

	std::string word, line, word2;
	// Parcours le fichier jusqu'au WorldBegin
	std::getline(pbrt_file, line);
	std::istringstream iss(line);
	iss >> word;
	while (strcmp(word.c_str(), "WorldBegin")) {
		std::getline(pbrt_file, line);
		iss.str(line);
		iss >> word;
	}
	
	// Parcours de WorldBegin
	while (std::getline(pbrt_file, line)) {
		iss.clear();
		iss.str(line);
		iss >> word;
		if (!strcmp(word.c_str(), "MakeNamedMaterial")) {
			std::string nameMaterial;
			for (int i = 0; i < 10; ++i) {
				iss >> word;
				if (i == 0) {
					std::string subWordTemp = word.c_str();
					for (int w = 0; w < subWordTemp.length() - 2; ++w)
						nameMaterial += subWordTemp[w + 1];
				}
			}
			float color1 = atof(word.c_str());
			iss >> word;
			float color2 = atof(word.c_str());
			iss >> word;
			float color3 = atof(word.c_str());
			current_material->setColor(glm::vec3(color1, color2, color3));
			material_library.addMaterial(current_material, nameMaterial);
		}
		if (!strcmp(word.c_str(), "Shape")) {
			std::string remaining;
			bool values = false;
			std::string param = extractNextParameter(line, remaining, &values);
			// Parser les valeurs
		}
	}
}

std::string PBRTLoader::extractNextParameter(std::string &line, std::string &remaining, bool *values) const {
	std::string temp = line.substr(line.find_first_of("\"") + 1);
	remaining = temp.substr(temp.find_first_of("\"") + 1);
	if (remaining.at(1) == '[') {
		*values = true;
	}
	remaining = remaining.substr(remaining.find_first_of("\""));
	temp = temp.substr(0, temp.find_first_of("\""));
	return temp;
}
