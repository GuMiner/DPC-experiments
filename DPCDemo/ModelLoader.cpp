#undef __SYCL_DEVICE_ONLY__ // Breaks Eigen used by igl.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <igl/readSTL.h>
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreorder-ctor"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wformat"
//#include <igl/opengl/glfw/Viewer.h>
#pragma clang diagnostic pop

#include <glm/vec3.hpp>

#include <igl/centroid.h>

#include "SimConstants.h"
#include "ModelLoader.h"

void ModelLoader::Load(std::string path, FanMesh& fanMesh)
{
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
    for (auto row : V.rowwise())
    {
        row[0] = FAN_SIZE_XY * ((row[0] - min[0]) / (max[0] - min[0])) + (FAN_CENTER_XY - FAN_SIZE_XY / 2.0f);
        row[1] = FAN_SIZE_XY * ((row[1] - min[1]) / (max[1] - min[1])) + (FAN_CENTER_XY - FAN_SIZE_XY / 2.0f);
        row[2] = scaleZ * ((row[2] - min[2]) / (max[2] - min[2])) + (FAN_CENTER_Z - sizeZ / 2.0f);
    }

    max = V.colwise().maxCoeff();
    min = V.colwise().minCoeff();
    std::cout << "  Rescaled mesh extents: (" <<
        min[0] << " - " << max[0] << ", " <<
        min[1] << " - " << max[1] << ", " <<
        min[2] << " - " << max[2] << ")" << std::endl;

    // Mass and centroid for physics
    Eigen::Vector3d center;
    double volume;
    igl::centroid(V, F, center, volume);
    fanMesh.centroid = glm::vec3(center[0], center[1], center[2]);
    fanMesh.mass = volume * FAN_DENSITY;

    // Expand and store vertices
    for (auto row : F.rowwise())
    {
        // TODO
    }

    std::cout << "Loaded!" << std::endl;
}

void ModelLoader::LoadAndRender()
{
    // many rows, only 3 columns for all of these.
    Eigen::MatrixXd V;
    Eigen::MatrixXd N;
    Eigen::MatrixXi F;
    igl::readSTL("test-fans/plane.stl", V, F, N);
    std::cout << V.rows() << " " << V.cols() << std::endl;
    std::cout << N.rows() << " " << N.cols() << std::endl;
    std::cout << F.rows() << " " << F.cols() << std::endl;

    //Eigen::SparseMatrix<double> M;
    //igl::massmatrix(V, F, igl::MASSMATRIX_TYPE_DEFAULT, M);



    //igl::opengl::glfw::Viewer viewer;
    //viewer.data().set_mesh(V, F);
    //viewer.launch();
}
