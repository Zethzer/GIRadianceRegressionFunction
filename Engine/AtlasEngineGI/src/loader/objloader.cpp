#include "objloader.h"
#include "material.h"
#include "model.h"
#include "scenegraphnode.h"
#include "scene.h"

OBJLoader::OBJLoader()
{

}

void OBJLoader::loadFile(const std::string &obj_path, Scene *scene, MaterialLibrary &material_library)
{
    std::string path = obj_path.substr(0, obj_path.find_last_of("/") + 1);

    std::string material_path = getMaterialPath(obj_path);
    getMaterials(path, material_path, material_library);

    std::vector<glm::vec3> positions;       //  Contiendra les positions de tous vertices
    std::vector<glm::vec3> normals;         //  Contiendra toutes les normales
    std::vector<glm::vec2> texture_coords;  //  Contiendra les coordonnées de texture de tous les vertices

    unsigned int indices_num = fillDataArrays(obj_path, positions, normals, texture_coords);

    addModels(material_library, scene, indices_num, positions, normals, texture_coords, obj_path, path);
}

/*
 * Récupère le nom du fichier .mtl
 * s'il y en a un
 * */
std::string OBJLoader::getMaterialPath(const std::string &obj_path) const
{
    std::string material_path,
                word,
                line;

    std::ifstream obj_file(obj_path.c_str());

    if(!(obj_file))
    {
        std::cerr << "OBJLOADER::GETMATERIALPATH::ERROR Problème lors de la lecture du fichier .obj" << std::endl;
        exit(1);
    }

    //  On saute les éventuels commentaires en début de fichier
    do
        std::getline(obj_file, line);
    while(line[0] == '#');

    //  Récupération du nom du fichier .mtl
    std::istringstream iss(line);
    if(iss >> word >> material_path)
    {
        //  .mtl présent
        if(!std::strcmp(word.c_str(), "mtllib"))
        {
            obj_file.close();
            return material_path;
        }
    }
}

/*
 * Ajoute à la library les matériaux du fichier .mtl
 * */
void OBJLoader::getMaterials(const std::string &path, const std::string &material_path, MaterialLibrary &material_library)
{
    if(material_path == "") //  Si pas de .mtl
    {
        if(material_library.hasMaterial("default"))
        {
            //  Matériau par défaut
            Material *material = new Material();
            material_library.addMaterial(material, "default");
        }
    }
    else
    {
        std::string material_full_path = path + material_path;
        std::ifstream material_file(material_full_path.c_str());

        if(!(material_file))
        {
            std::cerr << "OBJLOADER::GETMATERIALS::ERROR Problème lors de la lecture du fichier .mtl" << std::endl;
            exit(1);
        }

        std::string material_name,
                    word;

        Material *current_material;
        float R,G,B;

        while(material_file)
        {
            material_file >> word;

            //  Nouveau matériau
            if(!std::strcmp(word.c_str(), "newmtl"))
            {
                material_file >> material_name;
                current_material = new Material();
            }

            //  Couleur diffuse
            else if(!std::strcmp(word.c_str(), "Kd"))
            {
                material_file >> R >> G >> B;
                current_material->setColor(glm::vec3(R,G,B));

                material_library.addMaterial(current_material, material_name);
            }
        }
        material_file.close();
    }
}

/*
 * Parcours du .obj
 * Remplissage des vecteurs de
 * -positions
 * -normales
 * -coordonnées de texture
 * */
unsigned int OBJLoader::fillDataArrays(const std::string &obj_path, std::vector<glm::vec3> &position, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &texture_coords)
{
    unsigned faces_num = 0;

    std::ifstream obj_file;

    obj_file.open(obj_path.c_str());

    if(!(obj_file))
    {
        std::cerr << "OBJLOADER::FILLDATAARRAYS::ERROR Problème lors de la lecture du fichier .obj" << std::endl;
        exit(1);
    }

    std::string line,
                word,
                face1,
                face2,
                face3;
    float X,Y,Z;

    char letter;

    while(std::getline(obj_file, line))
    {
        std::istringstream iss(line);

        if(iss >> word >> X >> Y >> Z)
        {
            if(!strcmp(word.c_str(), "v"))          //  Vertex
                position.push_back(glm::vec3(X,Y,Z));

            else if(!strcmp(word.c_str(), "vn"))    //  Normale
                normals.push_back(glm::vec3(X,Y,Z));
        }
        else
        {
            iss.clear();
            iss.str(line);

            if(iss >> word >> X >> Y)
            {
                if(!strcmp(word.c_str(), "vt"))     //  Coordonnée de texture
                    texture_coords.push_back(glm::vec2(X,Y));
            }
            else
            {
                iss.clear();
                iss.str(line);

                if(iss >> letter >> face1 >> face2 >> face3 && line[0] == 'f') //  Face
                    faces_num += 3;
            }
        }
    }

    obj_file.close();

    if(texture_coords.empty())    //  Si les coordonnées de texture ne sont pas présentes dans le .obj
        texture_coords.push_back(glm::vec2(0.f));   //  Contiendra 1 élément par défaut

    return faces_num;
}

/*
 * Construction des Models
 * et ajout dans la scène
 * */
void OBJLoader::addModels(MaterialLibrary &material_library, Scene *scene, const unsigned int &indices_num, const std::vector<glm::vec3> positions, const std::vector<glm::vec3> normals, const std::vector<glm::vec2> texture_coords, const std::string &obj_path, const std::string &path) const
{
    SceneGraphRoot *root = new SceneGraphRoot("root", path);

    IndexTable index_table(indices_num, 0, 0, positions.size());

    GLboolean first_mesh = true;

    std::string line,
                word,
                material_name,
                face1,
                face2,
                face3;

    char letter;

    Material *current_material;

    std::ifstream obj_file;

    obj_file.open(obj_path.c_str());

    if(!(obj_file))
    {
        std::cerr << "OBJLOADER::ADDMODELS::ERROR Problème lors de la lecture du fichier .obj" << std::endl;
        exit(1);
    }

    while(std::getline(obj_file, line))
    {
        std::istringstream iss(line);

        if(iss >> letter >> face1 >> face2 >> face3)  //  Récupération des faces
        {
            if(letter == 'f') //  Ajout des face dans la table
            {
                index_table.addFace(face1);
                index_table.addFace(face2);
                index_table.addFace(face3);
            }
        }
        else
        {
            iss.clear();
            iss.str(line);

            if(iss >> word >> material_name)
            {
                if(!strcmp(word.c_str(), "usemtl")) //  Récupération du material
                    current_material = material_library.getMaterial(material_name);

                else if(!strcmp(word.c_str(), "o")) //  Changement de model
                {
                    if(first_mesh)
                        first_mesh = false;
                    else
                    {
                        std::vector<unsigned int> indices_vec;
                        std::vector<Vertex> vertices_vec;

                        index_table.buildMeshDatas(indices_vec, vertices_vec, positions, normals, texture_coords);

                        Mesh *mesh = new Mesh(vertices_vec, indices_vec);
                        Model *model = new Model(mesh, current_material);

                        SceneGraphNode *node = new SceneGraphNode(material_name, path);
                        node->addModel(model);

                        root->addChild(node);
                    }
                }
            }
        }
    }
    std::vector<unsigned int> indices_vec;
    std::vector<Vertex> vertices_vec;

    index_table.buildMeshDatas(indices_vec, vertices_vec, positions, normals, texture_coords);

    Mesh *mesh = new Mesh(vertices_vec, indices_vec);
    Model *model = new Model(mesh, current_material);

    SceneGraphNode *node = new SceneGraphNode(material_name, path);
    node->addModel(model);

    root->addChild(node);

    scene->addSceneGraphRoot(root);
}
