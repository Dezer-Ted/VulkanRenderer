//
// Created by theom on 09.08.2024.
//

#ifndef VULKANLAB01_SCENELOADER_H
#define VULKANLAB01_SCENELOADER_H

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include "nlohmann/json.hpp"
#include "CommandPool.h"

class Scene;
namespace dae {


    enum class MeshType {
        Mesh2D,
        Mesh3D
    };
    enum class MeshType2D {
        Circle,
        Rectangle
    };
    enum class MeshType3D {
        Cube,
        Plane,
        Obj
    };
    struct LoadableMesh {
        LoadableMesh() = default;
        LoadableMesh(const LoadableMesh& ) = default;
        LoadableMesh(LoadableMesh&&) = default;
        LoadableMesh& operator=(const LoadableMesh&) = default;
        LoadableMesh& operator=(LoadableMesh&& ) = default;
        ~LoadableMesh() = default;
        MeshType m_MeshType{};
        MeshType2D m_2DMeshType{};
        MeshType3D m_3DMeshType{};
        std::string m_ObjPath{};
        std::string m_AlbedoMap{};
        std::string m_RoughnessMap{};
        std::string m_NormalMap{};
        std::string m_SpecularMap{};
        glm::vec3 m_Position{};
        float m_Scale{1.f};
    };
    void to_json(nlohmann::json& j, const LoadableMesh& lm);
    void from_json(const nlohmann::json& j, LoadableMesh& lm);
    class SceneLoader {
    public:
        SceneLoader() = default;
        SceneLoader(const SceneLoader&) = delete;
        SceneLoader(SceneLoader&&) = delete;
        SceneLoader& operator=(const SceneLoader&) = delete;
        SceneLoader& operator=(SceneLoader&&) = delete;
        ~SceneLoader() = default;
        void SerializeObject();
        void
        DeserializeObjects(const std::string &jsonPath, Scene *scene, const CommandPool &pool, VkQueue graphicsQueue);
    private:
        void Init3DMesh(const LoadableMesh &loadableMesh, Scene *scene, const CommandPool &pool, VkQueue graphicsQueue);
        void Init2DMesh(const LoadableMesh &loadableMesh, Scene *scene);
    };
}


#endif //VULKANLAB01_SCENELOADER_H
