#include <iostream>
#include "Model.h"
#include "FootPrint.h"

using namespace std;

int main() {
    FootPrint footPrint("ground");
    footPrint.model.readModel(footPrint._projects_path + "scene_mesh.ply");
    footPrint.loadImages(footPrint._sfm_projects_path + "image_list.txt");
    footPrint.loadCameraParam(footPrint._data_path + "camera_param.txt");
    footPrint.loadMultipleCameraRts(footPrint._projects_path + "camera_pose.txt");
    footPrint.loadOpenPoseData(footPrint._projects_path + "open_pose.txt");
    footPrint.trackTargetPerson();
    footPrint.findFootPrint();
    footPrint.paintFootPrint();

    return 0;
}