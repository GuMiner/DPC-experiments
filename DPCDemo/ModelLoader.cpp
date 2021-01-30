#undef __SYCL_DEVICE_ONLY__ // Breaks Eigen used by igl.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <igl/readSTL.h>
#include <igl/centroid.h>
#pragma clang diagnostic pop

#if ENABLE_MODEL_LOADER_DEBUG_VIEWER
// This really, really doesn't like being built under clang.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreorder-ctor"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wformat"
#include <igl/opengl/glfw/Viewer.h>
#pragma clang diagnostic pop
#endif

#include <glm/vec3.hpp>
#include <glm/gtx/normal.hpp> 

#include "SimConstants.h"
#include "ModelLoader.h"

void ModelLoader::Load(std::string path, FanMesh* const fanMesh) {
    std::cout << "Loading \"" << path << "\"..." << std::endl;

    // <n> rows, 3 columns for all of these.
    Eigen::MatrixXd V;
    Eigen::MatrixXd N;
    Eigen::MatrixXi F;
    igl::readSTL(path, V, F, N);
    std::cout << "  Read in " <<
        V.rows() << " vertices, " <<
        N.rows() << " normals, and " <<
        F.rows() << " faces." << std::endl;

    // Rescale to the global center.
    Eigen::Vector3d max = V.colwise().maxCoeff();
    Eigen::Vector3d min = V.colwise().minCoeff();
    std::cout << "  Mesh extents: (" <<
        min[0] << " - " << max[0] << ", " <<
        min[1] << " - " << max[1] << ", " <<
        min[2] << " - " << max[2] << ")" << std::endl;

    // XY scaling factor is fixed (should be the same size in X and Y).
    // Compute Z scaling from an average of X and Y, just to be a bit safe in case of X & Y sizing differences.
    float scaleZ = (FAN_SIZE_XY / (max[0] - min[0]) + FAN_SIZE_XY / (max[1] - min[1])) / 2.0f;
    float sizeZ = scaleZ * (max[2] - min[2]);

    // scaling equation: FAN_SIZE_XY * ((x - min_x) / (max_x - min_x)) + FAN_START_XY
    for (std::ptrdiff_t i = 0; i < V.rows(); i++) {
       V(i, 0) = FAN_SIZE_XY * ((V(i, 0) - min[0]) / (max[0] - min[0])) + (FAN_CENTER_XY - FAN_SIZE_XY / 2.0f);
       V(i, 1) = FAN_SIZE_XY * ((V(i, 1) - min[1]) / (max[1] - min[1])) + (FAN_CENTER_XY - FAN_SIZE_XY / 2.0f);
       V(i, 2) = scaleZ * ((V(i, 2) - min[2]) / (max[2] - min[2])) + (FAN_CENTER_Z - sizeZ / 2.0f);
    }

    max = V.colwise().maxCoeff();
    min = V.colwise().minCoeff();
    std::cout << "  Rescaled mesh extents: (" <<
        min[0] << " - " << max[0] << ", " <<
        min[1] << " - " << max[1] << ", " <<
        min[2] << " - " << max[2] << ")" << std::endl;

    fanMesh->min = glm::vec3(min[0], min[1], min[2]);
    fanMesh->max = glm::vec3(max[0], max[1], max[2]);

    // Mass and centroid for physics
    Eigen::Vector3d center;
    double volume;
    igl::centroid(V, F, center, volume);
    fanMesh->centroid = glm::vec3(center[0], center[1], center[2]);
    fanMesh->mass = volume * FAN_DENSITY;

    // Expand and store vertices
    fanMesh->Vertices.clear();
    fanMesh->Normals.clear();
    for (std::ptrdiff_t i = 0; i < F.rows(); i++) {
        int firstIdx = F(i, 0);
        int secondIdx = F(i, 1);
        int thirdIdx = F(i, 2);
        glm::vec3 first = glm::vec3(V(firstIdx, 0), V(firstIdx, 1), V(firstIdx, 2));
        glm::vec3 second = glm::vec3(V(secondIdx, 0), V(secondIdx, 1), V(secondIdx, 2));
        glm::vec3 third = glm::vec3(V(thirdIdx, 0), V(thirdIdx, 1), V(thirdIdx, 2));
        
        fanMesh->Vertices.push_back(first);
        fanMesh->Vertices.push_back(second);
        fanMesh->Vertices.push_back(third);

        // Recompute normals as if X and Y aren't exactly the same, some distortion may have happened.
        // This shouldn't be the case, but the recomputation is trivially fast to do.
        fanMesh->Normals.push_back(glm::triangleNormal(first, second, third));
    }

    std::cout << "Loaded!" << std::endl;

#if ENABLE_MODEL_LOADER_DEBUG_VIEWER
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(V, F);
    viewer.launch();
#endif
}