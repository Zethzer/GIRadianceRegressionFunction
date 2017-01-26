#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "meshloader.h"

#include "openglincludes.h"

enum Flags
{
    aeNoFlag = 0x0,
    aeMissMesh = 0x1,
    aeMissLight = 0x2,
    aeMissCamera = 0x4,
};

class Scene;

class FileLoader
{
    public:
    FileLoader();
    
    /*GLboolean load(const std::string path, Scene *scene, MaterialLibrary &material_library, int flags = aeNoFlag);
    GLboolean load(const std::string path, SceneGraphNode *scene_graph_node, MaterialLibrary &material_library, int flags = aeNoFlag);
    void loadLights(const aiScene *ai_scene, Scene *scene);
    void loadCameras(const aiScene *ai_scene, Scene *scene);
    void loadMaterials(const aiScene *ai_scene, MaterialLibrary &material_library, const std::string &path);*/
    
    private:
    void processFlags(const int &flags);
    
    MeshLoader m_meshloader;
    
    GLboolean m_process_meshes;
    GLboolean m_process_lights;
    GLboolean m_process_cameras;
    
};

#endif // FILELOADER_H
