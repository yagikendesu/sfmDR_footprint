//
// Created by yagi on 18/07/24.
//

#include "FootPrint.h"
#include "basicFunction/basicFunction.h"
#include<fstream>
#include "OpenPosePerson.h"

using namespace std;

int FootPrint::loadCameraParam(std::string file_name) {
    std::ifstream ifs(file_name);
    std::string str;
    if (ifs.fail())
    {
        std::cout << "cannot open "<< file_name << std::endl;
        return -1;
    }

    cout << "[Camera parameter correctly loaded]:" << endl;
    vector<string> params;
    while (getline(ifs, str))
    {
        params = yagi::split(str, ' ');
        for(string tmp: params){
            this->cameraParam.push_back(stof(tmp));
        }
    }
    for (float tmp: this->cameraParam){
        cout << tmp << " ";
    }
    cout << endl;
    return 1;
}

int FootPrint::loadImages(string file_path) {

    //画像リストopen
    ifstream ifs(file_path);

    //imshowうまくいかない時用
    string line;
    int string_size = 0;

    // cv::namedWindow("image", CV_WINDOW_NORMAL);
    while (getline(ifs, line)) {

        //imshowがうまくいかないときここ原因(下4行をコメントアウト)
        if (string_size == 0 || (string_size + 1) == line.size()) {
            line.erase(line.size() - 1);
        }
        string_size = line.size();

        //ImageInfoに画像を格納していく
        ImageInfo image_info;

        //カラー、グレースケール,hsv
        cv::Mat image = cv::imread(line);

        //画像格納
        image_info.image = image;

        this->image_infos.push_back(image_info);

    }
}


int FootPrint::loadMultipleCameraRts(std::string file_name){
    ifstream ifs(file_name);
    string buf;

    if(!ifs.is_open()){
        std::cout << "cannot open "<< file_name << std::endl;
        return false;
    }

    cout << "[Camera pose file correctly loaded:]" << endl;
    int line_num = 0;
    int camera_num = 0;
    Camera camera_tmp;
    cv::Mat t_tmp = cv::Mat::zeros(3, 1, CV_64F);
    cv::Mat r_tmp = cv::Mat::zeros(3, 3, CV_64F);
    while(getline(ifs,buf)){
        if ((line_num % 14) == 5){
            camera_tmp.setId(camera_num);
            vector<string> translation_tmp  = yagi::split(buf, ' ');
            t_tmp.at<double>(0, 0) = stof(translation_tmp[0]);
            t_tmp.at<double>(0, 1) = stof(translation_tmp[1]);
            t_tmp.at<double>(0, 2) = stof(translation_tmp[2]);
            cv::Mat tmp = t_tmp.clone();
            camera_tmp.setT(tmp);

        }else if((line_num % 14) == 8){
            vector<string> rotation_tmp  = yagi::split(buf, ' ');
            r_tmp.at<double>(0, 0) = stof(rotation_tmp[0]);
            r_tmp.at<double>(0, 1) = stof(rotation_tmp[1]);
            r_tmp.at<double>(0, 2) = stof(rotation_tmp[2]);

        }else if((line_num % 14) == 9){
            vector<string> rotation_tmp  = yagi::split(buf, ' ');
            r_tmp.at<double>(1, 0) = stof(rotation_tmp[0]);
            r_tmp.at<double>(1, 1) = stof(rotation_tmp[1]);
            r_tmp.at<double>(1, 2) = stof(rotation_tmp[2]);

        }else if((line_num % 14) == 10){
            vector<string> rotation_tmp  = yagi::split(buf, ' ');
            r_tmp.at<double>(2, 0) = stof(rotation_tmp[0]);
            r_tmp.at<double>(2, 1) = stof(rotation_tmp[1]);
            r_tmp.at<double>(2, 2) = stof(rotation_tmp[2]);
            cv::Mat tmp = r_tmp.clone();
            camera_tmp.setR(tmp);

        }else if((line_num % 14) == 13){
            Camera tmp = camera_tmp;
            this->image_infos[camera_num].camera = tmp;
            camera_num++;
        }
        line_num++;
    }

    return 1;
}

int FootPrint::loadOpenPoseData(string file_name){
    //OpenPoseのデータ読みこみ
    ifstream ifs(file_name);
    string line;
    OpenPosePerson person;
    vector<OpenPosePerson> persons;
    vector<vector<OpenPosePerson>> allFramePersons;

    bool first_person = true;
    int frame_counter = 0;

    while (getline(ifs, line)) {

        vector<string> coords = yagi::split(line, ' ');

        if (coords.size() == 5) {

            if (!first_person) {

                OpenPosePerson dummy_person = person;
                persons.push_back(dummy_person);
                person.clearBodyCoord();

                if (coords[1] == "0") {
                    vector<OpenPosePerson> dummy_persons = persons;
                    allFramePersons.push_back(dummy_persons);

                    persons.clear();
                    frame_counter++;
                }
            }

        } else {
            person.setBodyCoord(coords);
            first_person = false;
        }
    }

    for (int i = 0; i < allFramePersons.size(); i++) {
        this->image_infos[i].persons = allFramePersons[i];
    }

}

//グローバル変数
struct mouseParam {
    int x;
    int y;
};
bool flag = false;
cv::Point2f tracking_runner_point;

//コールバック関数
void runnerClickCallBackFunc(int eventType, int x, int y, int flags, void *userdata) {
    switch (eventType) {
        case cv::EVENT_LBUTTONUP:
            std::cout << x << " , " << y << std::endl;
            tracking_runner_point.x = x;
            tracking_runner_point.y = y;
            flag = true;
    }
}

int FootPrint::trackTargetPerson(){
    vector<cv::Scalar> colors;
    yagi::setColor(&colors);

    //最初のフレームで対象人物クリック
    mouseParam mouseEvent;
    string window_name = "click target person";
    cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);
    cv::setMouseCallback(window_name, runnerClickCallBackFunc, &mouseEvent);

    cv::Mat image = this->image_infos[0].image.clone();
    while (1) {
        cv::imshow(window_name, image);
        int key = cv::waitKey(1);

        if (flag) {
            //click point格納
            flag = false;
            cv::circle(image, tracking_runner_point, 2, colors[0], 2);
        }

        if (key == 'q')
            break;
    }

    int frameID = 0;
    cv::Point2f prePt = tracking_runner_point;
    OpenPosePerson preHb;

    for (ImageInfo im : image_infos) {
        float minDist = 0;
        int minId = 0;
        int hbID = 0;
        bool target_found = false;
        cv::Point2f minPt;
        OpenPosePerson minHb;
        cv::Mat dum = im.image.clone();
        for (OpenPosePerson hb: im.persons) {

            vector<cv::Point2f> bodyCoords = hb.getBodyCoord();
            if (target_found == false) {
                for (int i = 0; i < bodyCoords.size(); i++) {
                    if ((i == 0) || (i == 14) || (i == 15) || (i == 16) || (i == 17)) {
                        float dist = yagi::calc2PointDistance(bodyCoords[i], prePt);
                        if (minDist > dist) {
                            minDist = dist;
                            minId = hbID;
                            minPt = bodyCoords[i];
                            minHb = hb;
                        }
                    }
                }
            } else {
                float sum_dist = 0;
                int parts_id = 0;
                int parts_num = 0;

                for (int i = 0; i < bodyCoords.size(); i++) {
                    for (cv::Point2f pre_pt: preHb.getBodyCoord()) {
                        if ((parts_id != 4) && (parts_id != 7) && (parts_id != 10) && (parts_id != 13)) {
                            if ((bodyCoords[i].x != 0) && (pre_pt.x != 0)) {
                                parts_num++;
                                sum_dist += yagi::calc2PointDistance(bodyCoords[i], pre_pt);
                            }
                        }
                    }
                }
                sum_dist /= parts_num;

                if (sum_dist < minDist) {
                    minDist = sum_dist;
                    minId = hbID;
                    minPt = im.persons[minId].getBodyCoord()[0];
                    minHb = hb;
                }
            }
            hbID++;


            if (minDist != 20.0) {
                image_infos[frameID].persons[minId].humanID = 1;
                prePt = minPt;
                preHb = minHb;
                target_found = true;
                for (cv::Point2f pt : im.persons[minId].getBodyCoord()) {
                    cv::circle(im.image, pt, 2, colors[3], 2);
                }

            }
        }

        frameID++;
        cv::imshow("targetRunner", im.image);
        cv::waitKey(10);

        if (frameID == (image_infos.size() - 1))
            break;
    }
}

int FootPrint::findFootPrint(){
    for(ImageInfo im: this->image_infos){
        for(OpenPosePerson person: im.persons){
            if (person.humanID == 1){
                cv::Point2f leftLeg = person.getBodyCoord()[10];
                cv::Point2f rightLeg = person.getBodyCoord()[13];

//                //足領域が3次元点群のどこにあたるか
//                vector<int> leftLegAreaID = this->findFootPrintAreaIn3D(leftLeg, im.camera);
//                vector<int> rightLegAreaID = this->findFootPrintAreaIn3D(rightLeg, im.camera);
//
//                //足領域を赤く塗りつぶす
//                this->paintFootPrint(leftLegAreaID);
//                this->paintFootPrint(rightLegAreaID);

                //足領域が3次元点群のどこにあたるか
                set<int> leftLegAreaID = this->findFootPrintAreaIn3DSet(leftLeg, im.camera);
                set<int> rightLegAreaID = this->findFootPrintAreaIn3DSet(rightLeg, im.camera);

                //setリストに追加
                for (int id: leftLegAreaID){
                    this->leftFootPrintID.insert(id);
                }
                for (int id: rightLegAreaID){
                    this->rightFootPrintID.insert(id);
                }
            }
        }
    }

    return 1;
}

cv::Point3f FootPrint::imagePointTo3dPoint(cv::Point2f point){
    cv::Point3f vector_to_world;
    vector_to_world.x = (point.x - this->cameraParam[2]) / this->cameraParam[0];
    vector_to_world.y = (point.y - this->cameraParam[5]) / this->cameraParam[4];
    vector_to_world.z = 1.0;
    return vector_to_world;
}

cv::Point2f FootPrint::worldPointToImagePoint(cv::Point3f point, Camera camera){
    //世界座標系からカメラ座標系への変換
    cv::Point3f camera_coord_point;
    cv::Mat R = camera.getR();
    camera_coord_point = R * point;

    //カメラ座標系から正規化画像座標系への変換
    cv::Point2f normalized_point;
    normalized_point.x = camera_coord_point.x / camera_coord_point.z;
    normalized_point.y = camera_coord_point.y / camera_coord_point.z;

    //正規化画像座標系から画像座標系への変換
    cv::Point2f image_point;
    image_point.x = (normalized_point.x * this->cameraParam[0]) + this->cameraParam[2];
    image_point.y = (normalized_point.x * this->cameraParam[4]) + this->cameraParam[5];

    return image_point;
}

vector<int> FootPrint::findFootPrintAreaIn3D(cv::Point2f point, Camera camera){
    vector<int> foot_area_vertices_id;
    for (int i = 0; i < this->model.vertices_num; i++) {
        cv::Point2f projected_point = this->worldPointToImagePoint(this->model.vertices[0], camera);
        if (yagi::calc2PointDistance(point, projected_point) < 10){
            foot_area_vertices_id.push_back(i);
        }
    }
    return foot_area_vertices_id;
}

set<int> FootPrint::findFootPrintAreaIn3DSet(cv::Point2f point, Camera camera){
    set<int> foot_area_vertices_id;
    for (int i = 0; i < this->model.vertices_num; i++) {
        cv::Point2f projected_point = this->worldPointToImagePoint(this->model.vertices[0], camera);
        if (yagi::calc2PointDistance(point, projected_point) < 10){
            foot_area_vertices_id.insert(i);
        }
    }
    return foot_area_vertices_id;
}

int FootPrint::paintFootPrint() {
    //plyfile出力

    //header
    ofstream outputfile(this->_projects_path + "/result/result.ply");
    outputfile << "ply" << endl;
    outputfile << "format ascii 1.0" << endl;
    outputfile << "comment VCGLIB generated" << endl;
    outputfile << "element vertex " + this->model.vertices_num << endl;
    outputfile << "property float x" << endl;
    outputfile << "property float y" << endl;
    outputfile << "property float z" << endl;
    outputfile << "property uchar red" << endl;
    outputfile << "property uchar green" << endl;
    outputfile << "property uchar blue" << endl;
    outputfile << "property uchar alpha" << endl;
    outputfile << "element face " + this->model.faces_num << endl;
    outputfile << "property list uchar int vertex_indices" << endl;
    outputfile << "end_header" << endl;

    for (int i = 0; i < this->model.vertices_num; i++){

        if( this->rightFootPrintID.find(i) != this->rightFootPrintID.end() )
        {
            outputfile  << this->model.vertices[i].x << " "
                        << this->model.vertices[i].y << " "
                        << this->model.vertices[i].z << " "
                        << 255 << " "
                        << 0   << " "
                        << 0   << " "
                        << 255 << endl;
        }else{
            outputfile  << this->model.vertices[i].x << " "
                        << this->model.vertices[i].y << " "
                        << this->model.vertices[i].z << " "
                        << 255 << " "
                        << 255 << " "
                        << 255 << " "
                        << 255 << endl;
        }

    }

    for (int i = 0; i < this->model.faces_num; i++){

        outputfile  << "3" << " "
                    << this->model.faces[i].verticies[0] << " "
                    << this->model.faces[i].verticies[1] << " "
                    << this->model.faces[i].verticies[2] << endl;

    }
    outputfile.close();
    return 1;

}


//            camera_tmp->t.push_back(stof(translation_tmp[0]));
//            camera_tmp->t.push_back(stof(translation_tmp[1]));
//            camera_tmp->t.push_back(stof(translation_tmp[2]));

//            camera_tmp->R.push_back(stof(rotation_tmp[0]));
//            camera_tmp->R.push_back(stof(rotation_tmp[1]));
//            camera_tmp->R.push_back(stof(rotation_tmp[2]));