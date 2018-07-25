//
// Created by yagi on 18/07/25.
//

#ifndef SFMDR_FOOTPRINT_OPENPOSEPERSON_H
#define SFMDR_FOOTPRINT_OPENPOSEPERSON_H

#include <iostream>
#include <opencv2/opencv.hpp>

class OpenPosePerson {
public:
    OpenPosePerson() {};
    ~OpenPosePerson() {};

    void setBodyCoord(std::vector<std::string> coord);
    std::vector<cv::Point2f> getBodyCoord();
    void clearBodyCoord();
    cv::Rect getMaskRect();
    int humanID = 100;
    int frameID;
    std::vector<cv::Point2f> _body_parts_coord;
};


#endif //SFMDR_FOOTPRINT_OPENPOSEPERSON_H
