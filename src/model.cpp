#include "model.hpp"

#include "material.hpp"
#include "mesh.hpp"

#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <iostream>

// ctor - Takes in unique_ptr references and moves them.
// Question: Is it better to do the std::move here, or do it when we call the
// ctor in the first place?
Model::Model(std::shared_ptr<Mesh> mesh, std::unique_ptr<Material>&& material) :
    // std::move is still required here, otherwise it would just be a copy, defeating the whole point
    // of passing mesh and material by rvalue ref in the first place
    mesh(mesh),
    material(std::move(material))
{}

Model::~Model() {}

Model::Model(Model&& other) {
    mesh = std::move(other.mesh);
    material = std::move(other.material);
    rotation = std::move(other.rotation);
    scale = std::move(other.scale);
    position = std::move(other.position);
    dirty = std::move(other.dirty);
}

Model& Model::operator=(Model&& other) {
    mesh = std::move(other.mesh);
    material = std::move(other.material);
    rotation = std::move(other.rotation);
    scale = std::move(other.scale);
    position = std::move(other.position);
    dirty = std::move(other.dirty);

    return *this;
}

void Model::setColor(glm::vec3 color) {
    material->setColor(color);
}

void Model::toggleEmissive(bool value) {
    material->toggleEmissive(value);
}

void Model::toggleBlinnPhongShading(bool value) {
    material->toggleBlinnPhongShading(value);
}

void Model::setEmissiveColor(glm::vec3 color) {
    material->setEmissiveColor(color);
}

void Model::setEmissiveStrength(float strength) {
    material->setEmissiveStrength(strength);
}

void Model::setEmissiveColorAndStrength(glm::vec3 color, float strength) {
    material->setEmissiveColorAndStrength(color, strength);
}

void Model::setLights(const std::vector<std::shared_ptr<Light>>& lights) {
    material->setLights(lights);
}

void Model::setProjectionAndViewMatrices(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    material->setProjectionAndViewMatrices(projectionMatrix, viewMatrix);
}

void Model::applyModelMatrix() {
    if (!dirty) {
        return;
    }

    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = modelMatrix * glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);

    material->setModelMatrix(modelMatrix);

    dirty = false;
}

void Model::draw() const {
    glUseProgram(material->getProgram());

    auto side = material->getSide();

    // TODO: Support both sides
    if (side == Side::BACK) {
        glCullFace(GL_FRONT);
    } else {
        glCullFace(GL_BACK);
    }

    glBindVertexArray(mesh->getVertexArrayObject());
    glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexCount());

    // set back to BACK
    glCullFace(GL_BACK);

    glUseProgram(0);
}
