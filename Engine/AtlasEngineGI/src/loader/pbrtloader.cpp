#include "pbrtloader.h"
#include "material.h"
#include "model.h"
#include "scenegraphnode.h"
#include "scene.h"
#include "pointlight.h"

PBRTLoader::PBRTLoader() {
	countVertex = 0;
}

void PBRTLoader::loadFile(const std::string &pbrt_path, Scene *scene, MaterialLibrary &material_library) {
	std::string path = pbrt_path.substr(0, pbrt_path.find_last_of("/") + 1);
	
	addModels(material_library, scene, pbrt_path, path);
}

void PBRTLoader::addModels(MaterialLibrary &material_library, Scene *scene, const std::string &pbrt_path, const std::string &path) {
	SceneGraphRoot *root = new SceneGraphRoot("root", path);
	std::vector<unsigned int> indices;		// Contiendra les indices
	std::vector<Vertex> vertices;			// Contiendra les vertices
	std::vector<glm::vec3> positions;       //  Contiendra les positions de tous vertices
	std::vector<glm::vec3> normals;         //  Contiendra toutes les normales
	std::vector<glm::vec2> texture_coords;  //  Contiendra les coordonnées de texture de tous les vertices
	// Material
	Material *current_material = new Material();
	float color1;
	float color2;
	float color3;
	// Light
	glm::vec3 lightColors;
	glm::vec3 lightPosition;
	// File
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
	int countNode = 0;
	while (std::getline(pbrt_file, line)) {
		iss.clear();
		iss.str(line);
		iss >> word;
		std::cout << "Debug node " << countNode << ": " << std::endl;
		// Propriétés des matériaux
		if (!strcmp(word.c_str(), "MakeNamedMaterial")) {
			std::string nameMaterial;
			for (int i = 0; i < 10; ++i) {
				iss >> word;
				// Creation du nom du materiau pour la material library
				if (i == 0) {
					std::string subWordTemp = word.c_str();
					for (int w = 0; w < subWordTemp.length() - 2; ++w)
						nameMaterial += subWordTemp[w + 1];
				}
			}
			// Extraction des couleurs du matériaux
			float color1 = atof(word.c_str());
			iss >> word;
			float color2 = atof(word.c_str());
			iss >> word;
			float color3 = atof(word.c_str());
			std::cout << "Name: " << nameMaterial << std::endl;
			std::cout << "Colors : " << color1 << " " << color2 << " " << color3 << std::endl;
			current_material->setColor(glm::vec3(1, 1, 1));
			//current_material->setColor(glm::vec3(color1, color2, color3));
			material_library.addMaterial(current_material, nameMaterial);
		}
		// Forme géométriques et propriétés
		if (!strcmp(word.c_str(), "Shape")) {
			std::string remaining = line, param, values;
			bool hasValues = false;
			int pos = remaining.find_first_of("\"");
			// Découpage et gestion de la ligne pour chaque propriété
			while (pos != std::string::npos){
				param = extractNextParameter(remaining, values, &hasValues, &pos);
				if (!strcmp(param.c_str(), "integer indices")) // Indices
					indices = parseIntegerValues(values);
				if (!strcmp(param.c_str(), "point P")) // Positions
					positions = parseManyVec3Values(values);
				if (!strcmp(param.c_str(), "normal N")) // Normales
					normals = parseManyVec3Values(values);
				if (!strcmp(param.c_str(), "float uv")) // Textures_coord
					texture_coords = parseManyVec2Values(values);
			}
			/*std::cout << "Indices: ";
			for (int i = 0; i < indices.size(); ++i)
				std::cout << indices[i] << " ";
			std::cout << std::endl;*/
			/*std::cout << "Positions: ";
			for (int i = 0; i < positions.size(); ++i)
				displayVec3(positions[i]);
			std::cout << "Normales: ";
			for (int i = 0; i < normals.size(); ++i)
				displayVec3(normals[i]);
			for (int i = 0; i < texture_coords.size(); ++i)
				displayVec2(texture_coords[i]);*/
			// Remplissage du tableau de vertices
			buildMeshData(vertices, indices, positions, normals, texture_coords);
			// Ajout du modèle à la scène avec son mesh
			Mesh *mesh = new Mesh(vertices,indices, false);
			Model *model = new Model(mesh, current_material);
			SceneGraphNode *node = new SceneGraphNode(("node" + std::to_string(countNode)).c_str(), model);
			root->addChild(node);
			++countNode;
		}
		// Différents paramètres : Ne gère actuellement que les lumières style pointLight
		if (!strcmp(word.c_str(), "AttributeBegin")) {
			std::getline(pbrt_file, line);
			iss.clear();
			iss.str(line);
			iss >> word;
			if (!strcmp(word.c_str(), "LightSource")) {
				// Extraction des paramètres
				std::string remaining = line, param, values;
				bool hasValues = false;
				int pos = remaining.find_first_of("\"");
				while (pos != std::string::npos) {
					param = extractNextParameter(remaining, values, &hasValues, &pos);
					if (!strcmp(param.c_str(), "rgb I"))
						lightColors = parseVec3Values(values);
					if (!strcmp(param.c_str(), "point from"))
						lightPosition = parseVec3Values(values);
				}
				// Création de la pointLight et ajout à la scène
				PointLight *pointLight = new PointLight(lightColors, 1.0f, lightPosition); // Intensité par défaut à 1
				scene->addPointLight(pointLight);
			}
		}
	}
	scene->addSceneGraphRoot(root);
}

void PBRTLoader::buildMeshData(std::vector<Vertex> &vertices, const std::vector<unsigned int> indices, const std::vector<glm::vec3> positions,
	const std::vector<glm::vec3> normals, const std::vector<glm::vec2> texture_coords) {

	for (int i = 0; i < indices.size(); ++i) {
		Vertex v;
		v.Position = positions[indices[i]];
		v.Normal = normals[indices[i]];
		v.TexCoords = texture_coords[indices[i]];
		/*std::cout << "Vertex " << countVertex << ":" << std::endl;
		std::cout << "Positions: "; displayVec3(v.Position);
		std::cout << "Normal: "; displayVec3(v.Normal);
		std::cout << "TexCoords: "; displayVec2(v.TexCoords);*/
		vertices.push_back(v);
		countVertex++;
	}
	countVertex = 0;
}

std::string PBRTLoader::extractNextParameter(std::string &line, std::string &valuesString, bool *values, int *pos) {
	std::string temp = line.substr(line.find_first_of("\"") + 1);
	line = temp.substr(temp.find_first_of("\"") + 1);
	if (line.at(1) == '[') {
		*values = true;
		valuesString = extractValues(line);
	}
	int position = line.find_first_of("\"");
	*pos = position;
	if(position != std::string::npos)
		line = line.substr(position);
	temp = temp.substr(0, temp.find_first_of("\""));
	return temp;
}

std::string PBRTLoader::extractValues(std::string &line) {
	std::string temp = line.substr(line.find_first_of("[") + 1);
	temp = temp.substr(0, temp.find_first_of("]"));
	return temp;
}

std::vector<unsigned int> PBRTLoader::parseIntegerValues(std::string &values) {
	std::vector<unsigned int> temp;
	unsigned int buffer;
	std::stringstream ss(values);
	while (ss >> buffer)
		temp.push_back(buffer);
	return temp;
}

std::vector<float> PBRTLoader::parseFloatValues(std::string &values) {
	std::vector<float> temp;
	float buffer;
	std::stringstream ss(values);
	while (ss >> buffer)
		temp.push_back(buffer);
	return temp;
}

glm::vec3 PBRTLoader::parseVec3Values(std::string &values) {
	std::vector<float> buffer = parseFloatValues(values);
	return glm::vec3(buffer[0], buffer[1], buffer[2]);
}

std::vector<glm::vec3> PBRTLoader::parseManyVec3Values(std::string &values) {
	std::vector<glm::vec3> temp;
	float x, y, z;

	std::stringstream ss(values);
	while (ss >> x >> y >> z)
		temp.push_back(glm::vec3(x, y, z));
	return temp;
}

std::vector<glm::vec2> PBRTLoader::parseManyVec2Values(std::string &values) {
	std::vector<glm::vec2> temp;
	float x, y;

	std::stringstream ss(values);
	while (ss >> x >> y)
		temp.push_back(glm::vec2(x, y));
	return temp;
}

void PBRTLoader::displayVec3(glm::vec3 t) {
	std::cout << "{ " << t.x << " " << t.y << " " << t.z << " }" << std::endl;
}

void PBRTLoader::displayVec2(glm::vec2 t) {
	std::cout << "{ " << t.x << " " << t.y << " }" << std::endl;
}
