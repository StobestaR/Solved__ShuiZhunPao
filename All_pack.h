using namespace cv;
using namespace std;

string CHANGE(string a) {  //矫正地址
    for (int i = 0; i < a.length(); i++) {
        if (a[i] == '\\') a[i] = '/';
    }
    return a;
}

bool RIGHT(string a) {  //判断尾缀是否正确
    int k = a.length();
    if ((a.substr(k - 4, 4)) == ".jpg" ||
        (a.substr(k - 4, 4)) == ".bmp" ||
        (a.substr(k - 4, 4)) == ".png" ||
        (a.substr(k - 5, 5)) == ".jpeg")
        return true;
    else
        return false;
}

class dmc {
private:
    Mat img;  //需要标定的图像
    double Real_Len;  //图像中物品实际的长度（微米）
public:
    Point2f center;
    float radius = 0;
    dmc(Mat YT) :Real_Len(33970) {
        Mat a; cv::cvtColor(YT, a, CV_BGR2GRAY);  //读入灰度图像
        Mat b; dlsBaseHistogram(a, b, 0.9, 0.665, 200);  //DLS灰度变换
        threshold(b, img, 250, 255, ThresholdTypes::THRESH_BINARY);  //二值化
    }
    double Biaoding();  //计算标定数值，其中返回 -1 未标定成功 
    ~dmc() {}
};

double dmc::Biaoding() {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    Mat imgcontours;

    findContours(img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);  //轮廓识别
    if (contours.size() > 0) {
        double maxarea = 0;
        int maxareaidx = 0;
        for (int index = contours.size() - 1; index >= 0; index--) {
            double tmparea = fabs(contourArea(contours[index]));
            if (tmparea > maxarea) {
                maxarea = tmparea;
                maxareaidx = index;
            }
        }
        minEnclosingCircle(contours[maxareaidx], center, radius);  //识别最小外接圆

        return (Real_Len / ((double)radius * 2));
    }
    else return -1;
}

class len {
private:
    Mat img;  //需要测算的图像
    double _BDZ;  //标定值
public:
    Mat imgout;  //输出的图像
    len(Mat YT, double DMCN) :_BDZ(DMCN), imgout(YT) {}
    Point2f jsLEN(); //计算刻线长度，其中返回（0，0）表示未计算成功
    ~len() {}
};

Point2f len::jsLEN() {
    Point2f fh;
    int xinhao = 0;  //信号标志位，如果识别到了需要测算的矩形，就返回1，继续执行
    fh.x = 0; fh.y = 0;

    Mat a; cvtColor(imgout, a, CV_BGR2GRAY);
    Mat b; dlsBaseHistogram(a, b, 0.9, 0.665, 200);  //DLS灰度变换
    Mat c; bilateralFilter(b, c, 20, 20, 25);  //双边滤波  中50
    threshold(c, img, 250, 255, ThresholdTypes::THRESH_BINARY);  //二值化


    vector<vector<Point>> contours;           //筛选轮廓
    vector<Vec4i> hierarchy;
    findContours(img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));  //轮廓识别
    Mat Final_Screen(img.size(), CV_8UC3, Scalar(0, 0, 0));
    for (int i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(255, 255, 255);
        double area1 = contourArea(contours[i]);
        if (area1 > 130000 && area1 < 150000) {  //面积筛选
            drawContours(Final_Screen, contours, i, color, 1, 8, hierarchy, 0, Point(0, 0));
            xinhao = 1;
        }
    }

    if (xinhao == 1) {
        Mat Final_Screen_R;                         //识别4顶点
        cvtColor(Final_Screen, Final_Screen_R, CV_BGR2GRAY);
        vector<vector<Point>> roi_point;
        findContours(Final_Screen_R, roi_point, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        vector<Point> Screen_4_Point;
        auto i = roi_point.begin();
        approxPolyDP(*i, Screen_4_Point, 200, 1);

        if (!Screen_4_Point.empty()) {
            double AVERAGE_X = 0;                      //确定左右点，标示
            for (auto& a : Screen_4_Point) AVERAGE_X += (double)a.x;
            AVERAGE_X /= Screen_4_Point.size();

            vector<Point> Point_Left, Point_Right;  //四顶点分类
            for (auto& a : Screen_4_Point) {
                if (a.x < AVERAGE_X)
                    Point_Left.push_back(a);
                else if (a.x >= AVERAGE_X)
                    Point_Right.push_back(a);

                circle(imgout, a, 1, Scalar(255, 0, 0), 2), circle(imgout, a, 7, Scalar(255, 0, 0), 2);  //画点
            }

            line(imgout, Point_Left[0], Point_Left[1], Scalar(0, 0, 255), 2, CV_AA);  //画线
            line(imgout, Point_Right[0], Point_Right[1], Scalar(0, 0, 255), 2, CV_AA);

            double Length_Left = 0, Length_Right = 0;  //计算左右刻线长度
            Length_Left = sqrt(pow(abs(Point_Left[0].x - Point_Left[1].x), 2) + pow(abs(Point_Left[0].y - Point_Left[1].y), 2));
            Length_Right = sqrt(pow(abs(Point_Right[0].x - Point_Right[1].x), 2) + pow(abs(Point_Right[0].y - Point_Right[1].y), 2));
            fh.x = Length_Left * _BDZ / (double)1000;
            fh.y = Length_Right * _BDZ / (double)1000;

            string Str_Length_Left, Str_Length_Right;  //标注长度文本
            Str_Length_Left = to_string(fh.x) + "mm";
            Str_Length_Right = to_string(fh.y) + "mm";
            Point2f POL, POR;
            POL.x = ((double)Point_Left[0].x + (double)Point_Left[1].x) / (double)2 + 5;
            POL.y = ((double)Point_Left[0].y + (double)Point_Left[1].y) / (double)2;
            POR.x = ((double)Point_Right[0].x + (double)Point_Right[1].x) / (double)2 + 5;
            POR.y = ((double)Point_Right[0].y + (double)Point_Right[1].y) / (double)2;
            putText(imgout, Str_Length_Left.c_str(), POL, 0, 1.0, Scalar(0, 255, 0), 2, 8);
            putText(imgout, Str_Length_Right.c_str(), POR, 0, 1.0, Scalar(0, 255, 0), 2, 8);
        }
    }
    return fh;
}