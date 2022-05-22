using namespace cv;
using namespace std;

string CHANGE(string a) {  //������ַ
    for (int i = 0; i < a.length(); i++) {
        if (a[i] == '\\') a[i] = '/';
    }
    return a;
}

bool RIGHT(string a) {  //�ж�β׺�Ƿ���ȷ
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
    Mat img;  //��Ҫ�궨��ͼ��
    double Real_Len;  //ͼ������Ʒʵ�ʵĳ��ȣ�΢�ף�
public:
    Point2f center;
    float radius = 0;
    dmc(Mat YT) :Real_Len(33970) {
        Mat a; cv::cvtColor(YT, a, CV_BGR2GRAY);  //����Ҷ�ͼ��
        Mat b; dlsBaseHistogram(a, b, 0.9, 0.665, 200);  //DLS�Ҷȱ任
        threshold(b, img, 250, 255, ThresholdTypes::THRESH_BINARY);  //��ֵ��
    }
    double Biaoding();  //����궨��ֵ�����з��� -1 δ�궨�ɹ� 
    ~dmc() {}
};

double dmc::Biaoding() {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    Mat imgcontours;

    findContours(img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);  //����ʶ��
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
        minEnclosingCircle(contours[maxareaidx], center, radius);  //ʶ����С���Բ

        return (Real_Len / ((double)radius * 2));
    }
    else return -1;
}

class len {
private:
    Mat img;  //��Ҫ�����ͼ��
    double _BDZ;  //�궨ֵ
public:
    Mat imgout;  //�����ͼ��
    len(Mat YT, double DMCN) :_BDZ(DMCN), imgout(YT) {}
    Point2f jsLEN(); //������߳��ȣ����з��أ�0��0����ʾδ����ɹ�
    ~len() {}
};

Point2f len::jsLEN() {
    Point2f fh;
    int xinhao = 0;  //�źű�־λ�����ʶ������Ҫ����ľ��Σ��ͷ���1������ִ��
    fh.x = 0; fh.y = 0;

    Mat a; cvtColor(imgout, a, CV_BGR2GRAY);
    Mat b; dlsBaseHistogram(a, b, 0.9, 0.665, 200);  //DLS�Ҷȱ任
    Mat c; bilateralFilter(b, c, 20, 20, 25);  //˫���˲�  ��50
    threshold(c, img, 250, 255, ThresholdTypes::THRESH_BINARY);  //��ֵ��


    vector<vector<Point>> contours;           //ɸѡ����
    vector<Vec4i> hierarchy;
    findContours(img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));  //����ʶ��
    Mat Final_Screen(img.size(), CV_8UC3, Scalar(0, 0, 0));
    for (int i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(255, 255, 255);
        double area1 = contourArea(contours[i]);
        if (area1 > 130000 && area1 < 150000) {  //���ɸѡ
            drawContours(Final_Screen, contours, i, color, 1, 8, hierarchy, 0, Point(0, 0));
            xinhao = 1;
        }
    }

    if (xinhao == 1) {
        Mat Final_Screen_R;                         //ʶ��4����
        cvtColor(Final_Screen, Final_Screen_R, CV_BGR2GRAY);
        vector<vector<Point>> roi_point;
        findContours(Final_Screen_R, roi_point, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        vector<Point> Screen_4_Point;
        auto i = roi_point.begin();
        approxPolyDP(*i, Screen_4_Point, 200, 1);

        if (!Screen_4_Point.empty()) {
            double AVERAGE_X = 0;                      //ȷ�����ҵ㣬��ʾ
            for (auto& a : Screen_4_Point) AVERAGE_X += (double)a.x;
            AVERAGE_X /= Screen_4_Point.size();

            vector<Point> Point_Left, Point_Right;  //�Ķ������
            for (auto& a : Screen_4_Point) {
                if (a.x < AVERAGE_X)
                    Point_Left.push_back(a);
                else if (a.x >= AVERAGE_X)
                    Point_Right.push_back(a);

                circle(imgout, a, 1, Scalar(255, 0, 0), 2), circle(imgout, a, 7, Scalar(255, 0, 0), 2);  //����
            }

            line(imgout, Point_Left[0], Point_Left[1], Scalar(0, 0, 255), 2, CV_AA);  //����
            line(imgout, Point_Right[0], Point_Right[1], Scalar(0, 0, 255), 2, CV_AA);

            double Length_Left = 0, Length_Right = 0;  //�������ҿ��߳���
            Length_Left = sqrt(pow(abs(Point_Left[0].x - Point_Left[1].x), 2) + pow(abs(Point_Left[0].y - Point_Left[1].y), 2));
            Length_Right = sqrt(pow(abs(Point_Right[0].x - Point_Right[1].x), 2) + pow(abs(Point_Right[0].y - Point_Right[1].y), 2));
            fh.x = Length_Left * _BDZ / (double)1000;
            fh.y = Length_Right * _BDZ / (double)1000;

            string Str_Length_Left, Str_Length_Right;  //��ע�����ı�
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