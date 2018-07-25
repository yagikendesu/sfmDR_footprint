//
// Created by yagi on 18/07/24.
//

#ifndef SFMDR_FOOTPRINT_CAMERA_H
#define SFMDR_FOOTPRINT_CAMERA_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "OpenPosePerson.h"

class Camera {
public:
    int loadCameraRt(std::string file_name);

    void setR(cv::Mat R){
        _R = R;
    }
    void setT(cv::Mat T){
        _T = T;
    }
    void setId(int id){
        _id = id;
    }
    cv::Mat getR(){
        return _R;
    }
    cv::Mat getT(){
        return _T;
    }
    int getId(){
        return _id;
    }

private:
    int _id;
    cv::Mat _R;
    cv::Mat _T;
};


#endif //SFMDR_FOOTPRINT_CAMERA_H
