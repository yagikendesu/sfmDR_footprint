//
// Created by yagi on 18/07/24.
//

#ifndef SFMDR_FOOTPRINT_FOOTPRINT_H
#define SFMDR_FOOTPRINT_FOOTPRINT_H

#include "Model.h"
#include "ImageInfo.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>


class FootPrint {
public:
    FootPrint(std::string project_name){
        _project_name = project_name;
        _data_path = "../Data/";
        _projects_path = _data_path + "Projects/" + _project_name + "/";
        _sfm_projects_path = "/home/yagi/sfmDR/projects/" + _project_name + "/";
    };
    ~FootPrint(){};

    std::string _project_name;
    std::string _data_path;
    std::string _projects_path;
    std::string _sfm_projects_path;

    int loadCameraParam(std::string file_name);
    int loadImages(std::string file_name);
    int loadMultipleCameraRts(std::string file_name);
    int loadOpenPoseData(std::string file_name);
    int trackTargetPerson();
    int findFootPrint();
    int paintFootPrint();

    cv::Point3f imagePointTo3dPoint(cv::Point2f point);
    cv::Point2f worldPointToImagePoint(cv::Point3f point, Camera camera);
    std::vector<int> findFootPrintAreaIn3D(cv::Point2f point, Camera camera);
    std::set<int> findFootPrintAreaIn3DSet(cv::Point2f point, Camera camera);

    Model model;
    int imageWidth = 1920;
    int imageHeight = 1080;
private:

    std::vector<ImageInfo> image_infos;
    std::set<int> rightFootPrintID;
    std::set<int> leftFootPrintID;
    std::vector<float> cameraParam;
};


#endif //SFMDR_FOOTPRINT_FOOTPRINT_H
