//
// Created by theom on 09.08.2024.
//

#include <sstream>
#include <fstream>
#include "SceneLoader.h"
#include "Mesh3D.h"
#include "Mesh.h"
#include "Scene.h"

void dae::SceneLoader::SerializeObject() {
    std::stringstream stream;
    nlohmann::json json;
    std::vector<LoadableMesh> lms{};
    LoadableMesh lm{};
    lm.m_MeshType = MeshType::Mesh3D;
    lm.m_3DMeshType = MeshType3D::Obj;
    lm.m_ObjPath = "Models/vehicle.obj";
    lm.m_AlbedoMap = "Models/vehicle_diffuse.png";
    lm.m_NormalMap = "Models/vehicle_normal.png";
    lm.m_SpecularMap = "Models/vehicle_specular.png";
    lm.m_RoughnessMap = "Models/vehicle_gloss.png";
    lm.m_Position = glm::vec3{20,0,0};
    lms.push_back(lm);

    lm = LoadableMesh{};
    lm.m_MeshType = MeshType::Mesh3D;
    lm.m_3DMeshType = MeshType3D::Obj;
    lm.m_ObjPath = "Models/box.obj";
    lm.m_AlbedoMap = "Models/box_albedo.png";
    lm.m_NormalMap = "Models/box_normal.png";
    lm.m_Position = glm::vec3{-20,0,0};
    lms.push_back(lm);

    lm = LoadableMesh{};
    lm.m_MeshType = MeshType::Mesh2D;
    lm.m_2DMeshType = MeshType2D::Rectangle;
    lm.m_Position = glm::vec3{0.5,-0.5,0};
    lms.push_back(lm);

    lm = LoadableMesh{};
    lm.m_MeshType = MeshType::Mesh2D;
    lm.m_2DMeshType = MeshType2D::Circle;
    lm.m_Position = glm::vec3 {-0.5 ,-0.5,0};
    lm.m_Scale = 0.2f;

    lms.push_back(lm);

    lms.push_back(lm);
    json = lms;
    std::fstream fs{"Models/scene.json", std::ios::out};
    fs << json;
    fs.close();
}

void dae::SceneLoader::DeserializeObjects(const std::string &jsonPath, Scene *scene, const CommandPool &pool,
                                          VkQueue graphicsQueue) {
    std::ifstream jsonFile{jsonPath};
    nlohmann::json json;
    jsonFile >> json;
    std::vector<LoadableMesh> lms{};
    lms = json.template get<std::vector<dae::LoadableMesh>>();

    for (const auto& lm : lms) {

        if(lm.m_MeshType == MeshType::Mesh3D) {
            Init3DMesh(lm, scene, pool, graphicsQueue);
        }
        else {
            Init2DMesh(lm, scene);
        }
    }
}

void dae::SceneLoader::Init3DMesh(const LoadableMesh &loadableMesh, Scene *scene, const CommandPool &pool,
                                  VkQueue graphicsQueue) {
    std::unique_ptr<Mesh3D> mesh {std::make_unique<Mesh3D>()};


    switch (loadableMesh.m_3DMeshType) {

        case MeshType3D::Cube:
            mesh->InitCube(loadableMesh.m_Position, loadableMesh.m_Scale);

            break;
        case MeshType3D::Plane:
            mesh->InitPlane(loadableMesh.m_Position,loadableMesh.m_Scale);
            break;
        case MeshType3D::Obj:
            mesh->LoadModel(loadableMesh.m_ObjPath);
            break;
    }
    mesh->LoadTexture(pool,graphicsQueue,
                      loadableMesh.m_AlbedoMap,
                      loadableMesh.m_NormalMap,
                      loadableMesh.m_RoughnessMap,
                      loadableMesh.m_SpecularMap);
    mesh->InitDescriptor();
    mesh->Translate(loadableMesh.m_Position);
    scene->Add3DMesh(std::move(mesh));

}

void dae::SceneLoader::Init2DMesh(const LoadableMesh &loadableMesh, Scene *scene) {
    std::unique_ptr<Mesh> mesh{};
    if(loadableMesh.m_2DMeshType == MeshType2D::Rectangle) {
        mesh  = std::make_unique<Mesh>(loadableMesh.m_Position,loadableMesh.m_Scale,loadableMesh.m_Scale);
    }
    else {
        mesh = std::make_unique<Mesh>(loadableMesh.m_Position,loadableMesh.m_Scale,24);
    }
    scene->Add2DMesh(std::move(mesh));
}

void dae::to_json(nlohmann::json &j, const dae::LoadableMesh &lm) {
    j = nlohmann::json {
            {"MeshType", lm.m_MeshType},
            {"MeshType2D", lm.m_2DMeshType},
            {"MeshType3D",lm.m_3DMeshType},
            {"ObjPath", lm.m_ObjPath},
            {"AlbedoPath",lm.m_AlbedoMap},
            {"NormalPath", lm.m_NormalMap},
            {"RoughnessPath", lm.m_RoughnessMap},
            {"SpecularPath", lm.m_SpecularMap},
            {"XPos", lm.m_Position.x},
            {"YPos",lm.m_Position.y},
            {"ZPos",lm.m_Position.z},
            {"Scale",lm.m_Scale}
    };
}
void dae::from_json(const nlohmann::json &j, dae::LoadableMesh &lm) {
    j.at("MeshType").get_to(lm.m_MeshType);
    j.at("MeshType2D").get_to(lm.m_2DMeshType);
    j.at("MeshType3D").get_to(lm.m_3DMeshType);
    j.at("ObjPath").get_to(lm.m_ObjPath);
    j.at("AlbedoPath").get_to(lm.m_AlbedoMap);
    j.at("NormalPath").get_to(lm.m_NormalMap);
    j.at("RoughnessPath").get_to(lm.m_RoughnessMap);
    j.at("SpecularPath").get_to(lm.m_SpecularMap);
    j.at("XPos").get_to(lm.m_Position.x);
    j.at("YPos").get_to(lm.m_Position.y);
    j.at("ZPos").get_to(lm.m_Position.z);
    j.at("Scale").get_to(lm.m_Scale);
}
