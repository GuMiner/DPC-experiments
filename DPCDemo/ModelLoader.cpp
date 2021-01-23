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
#include <igl/opengl/glfw/Viewer.h>
#pragma clang diagnostic pop

#include "ModelLoader.h"

void ModelLoader::LoadAndRender()
{
    Eigen::MatrixXd V;
    Eigen::MatrixXd N;
    Eigen::MatrixXi F;
    igl::readSTL("test-fans/plane.stl", V, F, N);
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(V, F);
    viewer.launch();
}
