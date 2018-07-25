//
// Created by yagi on 18/07/24.
//

#include "Model.h"
#include <fstream>
#include "basicFunction/basicFunction.h"


using namespace std;

bool Model::readModel(string filename){
    ifstream ifs(filename);
    string buf;
    unsigned int i;

    if(!ifs.is_open()){
        std::cout << "cannot open "<< filename << std::endl;
        return false;
    }
    cout << "[.ply file correctly loaded]:" << endl;
    while(ifs >> buf){

        if(buf =="vertex"){
            ifs >> vertices_num;
            if(ifs.fail()){
                std::cout <<"error! vertices_num is not int"<<std::endl;
            }
            cout << "vertex num: " << vertices_num << endl;
        }
        if(buf =="face"){
            ifs >> faces_num;
            if(ifs.fail()){
                std::cout <<"error! faces_num is not int"<<std::endl;
            }
            cout << "face num: " << faces_num << endl;
        }
        if(buf == "end_header")
            break;
    }
    if(vertices_num==0 || faces_num==0){
        return false;
    }
    std::getline(ifs,buf);
    vertices.resize(vertices_num);

    vector<string> vertex;
    vector<string> face;
    for(i= 0;i<vertices_num;i++){
        std::getline(ifs,buf);
        vertex = yagi::split(buf, ' ');
        cv::Point3f tmp;
        tmp.x = stof(vertex[0]);
        tmp.y = stof(vertex[1]);
        tmp.z = stof(vertex[2]);
        vertices.push_back(tmp);
    }
    faces.resize(faces_num);
    for(i= 0;i<faces_num;i++){
        std::getline(ifs,buf);
        face = yagi::split(buf, ' ');
        faces[i].verticies.push_back(stoi(face[1]));
        faces[i].verticies.push_back(stoi(face[2]));
        faces[i].verticies.push_back(stoi(face[3]));
    }
    if(vertices.size() == vertices_num && faces.size() == faces_num){
        return true;
    }else{
        return false;
    }
}