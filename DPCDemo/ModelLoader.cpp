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

#include <igl/centroid.h>

#include "ModelLoader.h"

void ModelLoader::LoadAndRender()
{
    Eigen::MatrixXd V;
    Eigen::MatrixXd N;
    Eigen::MatrixXi F;
    igl::readSTL("test-fans/plane.stl", V, F, N);

    //Eigen::SparseMatrix<double> M;
    //igl::massmatrix(V, F, igl::MASSMATRIX_TYPE_DEFAULT, M);
    Eigen::Vector3d center;
    double volume;
    igl::centroid(V, F, center, volume);
    //igl::barycenter(V, F, BC);
    std::cout << center << std::endl;
    std::cout << volume << std::endl;
    std::cout << "hi" << std::endl;
    //igl::opengl::glfw::Viewer viewer;
    //viewer.data().set_mesh(V, F);
    //viewer.launch();
}
