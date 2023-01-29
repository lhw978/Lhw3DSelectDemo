#ifndef MESH_H
#define MESH_H
#include <vector>
//using namespace std;  //不推荐在.h头文件中 使用using，容易导致命名冲突

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;    // position
    glm::vec3 Normal;      // normal
    glm::vec2 TexCoords;   // texCoords
    glm::vec3 Tangent;     // tangent
    glm::vec3 Bitangent;   // bitangent
    int m_BoneIDs[MAX_BONE_INFLUENCE];     //bone indexes which will influence this vertex
    float m_Weights[MAX_BONE_INFLUENCE];   //weights from each bone
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:  
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;     
        setupMesh();   // now that we have all the required data, set the vertex buffers and its attribute pointers.
    }
  
    void Draw(Shader& shader)  // render the mesh
    {      
        unsigned int diffuseNr = 1, specularNr = 1, normalNr = 1, heightNr = 1;  // bind appropriate textures
        for (unsigned int i = 0; i < textures.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            std::string number;              // retrieve texture number (the N in diffuse_textureN)
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string
          
            shader.setInt(("material." + name + number).c_str(), i);  // now set the sampler to the correct texture unit
            glBindTexture(GL_TEXTURE_2D, textures[i].id);    // and finally bind the texture
        }
       
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);  // draw mesh
        glBindVertexArray(0);            //恢复默认       

        glActiveTexture(GL_TEXTURE0);    // always good practice to set everything back to defaults once configured.
    }

private:      
    void setupMesh()  // initializes all the buffer objects/arrays
    {    
        glGenVertexArrays(1, &VAO);    // create buffers/arrays
        glBindVertexArray(VAO);   

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);     //顶点数据： load data into vertex buffers
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);  //索引数据
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers     
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);   // vertex Positions
    
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));    // vertex normals
     
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));   // vertex texture coords
      
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));  // vertex tangent
      
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));  // vertex bitangent
     
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));   // ids
     
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));   // weights
        glBindVertexArray(0);     //恢复默认
    }

    std::vector<Vertex>       vertices;  // mesh Data
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    unsigned int VAO;
    unsigned int VBO, EBO;  // render data 
};
#endif