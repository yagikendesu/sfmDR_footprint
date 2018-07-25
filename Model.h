//
// Created by yagi on 18/07/24.
//

#ifndef SFMDR_FOOTPRINT_MODEL_H
#define SFMDR_FOOTPRINT_MODEL_H

#include <iostream>
#include <vector>
#include <opencv2/core/types.hpp>

class Model {
public:
    bool readModel(std::string filename);

    struct Face{
        std::vector<int> verticies;
    };

    int vertices_num;
    int faces_num;

    std::vector<cv::Point3f> vertices;
    std::vector<Face> faces;

private:


};


#endif //SFMDR_FOOTPRINT_MODEL_H
