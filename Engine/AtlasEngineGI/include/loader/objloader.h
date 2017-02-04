#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <string>
#include <vector>

#include "mesh.h"

class Material;
class Scene;
struct MaterialLibrary;

/*
 * Table d'indexation des indices
 * */
struct IndexTable
{
    /*
     * Element de la table d'indexation des indices
     * Contient, pour un vertex, les indices de sa coordonnée de texture et de sa normale
     * si deux vertices possèdent la même position, mais des normales ou coordonnées de texture différentes,
     * le prochain sera à l'indice next dans le tableau
     *
     * Il sera sous cette forme:
     *
     * Indices  position normale     coord tex   suivant
     *          _________________________________________
     *        0 |0       0           0           n       | \
     *        1 |1       1           0           0       |  |- Première partie
     *        2 |2       2           1           0       | /
     *          | ...                                    |
     *          |                                        |
     *        n |0       1           0           0       | \
     *          | ...                                    |  |- Deuxième partie (suivants)
     *          |________________________________________| /
     * */
    struct IndexTableElement
    {
        unsigned int vertex_index;
        unsigned int texture_coord_index;
        unsigned int normal_index;
        int next;

        IndexTableElement()
        {
            next = -1;   //  Convention : -1 : non parcouru, 0 : pas de suivant, autre : indice du suivant
        }

        bool sameElement(const unsigned int &t_c_i, const unsigned int &n_i)
        {
            return (texture_coord_index == t_c_i && normal_index == n_i);
        }
    };


    IndexTableElement *table;

    unsigned int first_first_part;  //  Premier de la première partie
    unsigned int last_first_part;   //  Dernier de la première partie
    unsigned int first_second_part; //  Premier de la deuxième partie
    unsigned int last_second_part;  //  Dernier de la deuxième partie

    unsigned int *indices;          //  Tableau qui contiendra les indices des vertices
    unsigned int mesh_start;       //  Début du mesh courant dans le tableau
    unsigned int mesh_end;         //  Fin du mesh courant

    IndexTable(const unsigned int &table_size, const unsigned int &f_f_p, const unsigned int &l_f_p, const unsigned int &f_s_p) :
        first_first_part(f_f_p),
        last_first_part(l_f_p),
        first_second_part(f_s_p),
        last_second_part(f_s_p),
        mesh_start(0),
        mesh_end(0)
    {
        table = new IndexTableElement[table_size];
        indices = new unsigned int[table_size];
    }

    ~IndexTable()
    {
        delete[] table;
        delete[] indices;
    }

    void changeModel()
    {
        ++last_first_part;
        first_first_part = last_first_part;
        first_second_part = last_second_part;
    }

    void addElement(const unsigned int &element_index, const unsigned int &normal_index, const unsigned int &texture_index)
    {
        table[element_index].next = 0;
        table[element_index].vertex_index = element_index;
        table[element_index].normal_index = normal_index;
        table[element_index].texture_coord_index = texture_index;

        indices[mesh_end++] = element_index;

        if(element_index > last_first_part)
            last_first_part = element_index;
    }

    void addNext(const unsigned int &element_index, const unsigned int &normal_index, const unsigned int &texture_index)
    {
        table[element_index].next = last_second_part;
        table[last_second_part].next = 0;
        table[last_second_part].vertex_index = table[element_index].vertex_index;
        table[last_second_part].normal_index = normal_index;
        table[last_second_part].texture_coord_index = texture_index;

        indices[mesh_end++] = last_second_part++;
    }

    inline bool hasVertex(const unsigned int &element_index) const
    {
        return table[element_index].next != -1;
    }

    inline bool hasNext(const unsigned int &element_index) const
    {
        return table[element_index].next != 0;
    }

    inline bool sameElement(const unsigned int &element_index, const unsigned int &texture_coord_index, const unsigned int &normal_index) const
    {
        return table[element_index].sameElement(texture_coord_index, normal_index);
    }

    /*
     * Ajout d'une face
     * de la forme "<position_index>/<texture_index>/<normal_index>"
     * */
    void addFace(std::string face)
    {
        unsigned int position_index = atoi(face.substr(0, face.find("/")).c_str()) - 1; //  Indice du vertex

        face = face.substr(face.find("/") + 1);
        unsigned int texture_index = (face[0] == '/')?0:atoi(face.substr(0, face.find("/")).c_str()) - 1;   //  Coordonnées de texture

        face = face.substr(face.find("/") + 1);
        unsigned int normal_index = atoi(face.c_str()) - 1;   //  Normale


        //  Ajout des indices dans le tableau
        if(hasVertex(position_index))    // Si existe déjà
        {
            bool found = false;

                                        //  Recherche d'un vertex avec même normale et coord de texture
            while(position_index != 0)  //  On le compare à tous les vertices de même indice (i.e. parcours des suivants)
            {
                if(sameElement(position_index, texture_index, normal_index)) // Trouvé
                {
                    //  On ajoute simplement l'indice
                    indices[mesh_end++] = position_index;
                    found = true;
                    break;
                }
                if(hasNext(position_index))
                    position_index = table[position_index].next;  // On considère le suivant
                else
                    break;
            }

            if(!found)
                addNext(position_index, normal_index, texture_index);
        }
        else
            addElement(position_index, normal_index, texture_index); // Si nouveau vertex
    }

    /*
     * Build indices vector and vertices vector
     * for the current mesh
     * */
    void buildMeshDatas(std::vector<unsigned int> &indices_vec, std::vector<Vertex> &vertices_vec, const std::vector<glm::vec3> positions, const std::vector<glm::vec3> normals, const std::vector<glm::vec2> texture_coords)
    {
        ++last_first_part;

        for(unsigned int i = mesh_start, offset = first_second_part - last_first_part; i < mesh_end; ++i)
        {
            if(indices[i] > last_first_part)    //  si seconde partie
                indices_vec.push_back(indices[i] - offset - first_first_part);
            else
                indices_vec.push_back(indices[i] - first_first_part);
        }

        for(unsigned int i = first_first_part; i < last_first_part; ++i)
        {
            Vertex v;
            v.Position = positions[i];
            v.Normal = normals[table[i].normal_index];
            v.TexCoords = texture_coords[table[i].texture_coord_index];

            vertices_vec.push_back(v);
        }

        for(unsigned int i = first_second_part; i < last_second_part; ++i)
        {
            Vertex v;
            v.Position = positions[table[i].vertex_index];
            v.Normal = normals[table[i].normal_index];
            v.TexCoords = texture_coords[table[i].texture_coord_index];

            vertices_vec.push_back(v);
        }

        first_first_part = last_first_part;
        first_second_part = last_second_part;
        mesh_start = mesh_end;
    }
};


class OBJLoader
{
public:
    OBJLoader();

    void loadFile(const std::string &obj_path, Scene *scene, MaterialLibrary &material_library);

private:
    std::string getMaterialPath(const std::string &obj_path) const;
    void getMaterials(const std::string &path, const std::string &material_path, MaterialLibrary &material_library);
    unsigned int fillDataArrays(const std::string &obj_path, std::vector<glm::vec3> &position, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &texture_coords);
    void addModels(MaterialLibrary &material_library, Scene *scene, const unsigned int &indices_num, const std::vector<glm::vec3> positions, const std::vector<glm::vec3> normals, const std::vector<glm::vec2> texture_coords, const std::string &obj_path, const std::string &path) const;
};

#endif // OBJLOADER_H
