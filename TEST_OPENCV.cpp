
#include "TEST_OPENCV.h"

using namespace std;
using namespace cv;

int main() {
    system("COLOR 70 ");
    cout << "---------------欢迎使用水准泡刻线长度测量软件v1.0---------------" << endl
        << "  支持指令：" << endl
        << "    1  输入文件并处理" << endl
        << "    2  保存文件" << endl
        << "    3  退出程序" << endl
        << "----------------------------------------------------------------" << endl
        << "  请按以下说明使用该程序，以免造成不必要的错误：" << endl
        << "    *  本程序支持 .jpg .jpeg .bmp .png 格式文件" << endl
        << "    *  输入需要测量的水准泡图像文件路径时，将文件后缀名一起输入" << endl
        << "    *  保存文件只能保存上一次处理完的图片，以往的图片将被销毁" << endl
        << "    *  预览图像过大可以拖动窗口边缘调节至合适大小" << endl
        << "----------------------------------------------------------------" << endl
        << "  请输入您的指令：";

    int _ZL; //指令
    Mat YT; //原图
    Mat SC; //输出图像
    string _Path; //路径字符串

    cin >> _ZL;
    while (_ZL != EOF) {
        if (_ZL == 3) { //退出
            cout << "    感谢使用，请按回车键退出！";
            getchar();
            getchar();
            break;
        }
        else if (_ZL == 2) { //保存文件
            if (SC.empty()) {
                cout << "    文件为空，无法保存！" << endl;
            }
            else {
                imwrite(_Path.substr(0, _Path.length() - 4) + " - 副本" + _Path.substr(_Path.length() - 4, 4), YT);
                cout << "    已在原地址将文件保存为副本" << endl;
            }
        }
        else if (_ZL == 1) { //图像识别
            cout << "    路径：";
            cin >> _Path;
            _Path = CHANGE(_Path);
            while (!RIGHT(_Path)) {
                cout << "    路径错误，请阅读说明重新输入！" << endl
                    << "    路径：";
                cin >> _Path;
                _Path = CHANGE(_Path);
            }
            YT = imread(_Path);
            if (YT.empty()) {
                cout << "    文件错误或损坏，读取失败！" << endl;
            }
            else {
                cout << "    读取文件成功" << endl;
                namedWindow("原始图像", WINDOW_NORMAL);
                imshow("原始图像", YT);
                waitKey(0);
                destroyAllWindows();
                cout << "    开始标定...";
                dmc DMC(YT);
                double DMCN = DMC.Biaoding();
                if (DMCN == -1)
                    cout << "标定失败，请更换图片" << endl;
                else {
                    cout << "标定成功" << endl << "      标定值：" << DMCN << "微米 / 像素" << endl
                        << "    开始测量...";

                    len LEN(YT, DMCN);
                    Point2f changdu = LEN.jsLEN();
                    
                    if(changdu.x==0 && changdu.y==0) 
                        cout << "测量失败,请更换图片" << endl;
                    else {
                        cout << "测量成功"<<endl
                            << "      左侧刻线长度：" << changdu.x << "mm" << endl
                            << "      右侧刻线长度：" << changdu.y << "mm" << endl;
                        SC = LEN.imgout;

                        Point a;
                        a.x = (int)DMC.center.x;
                        a.y = (int)DMC.center.y;
                        int r = DMC.radius;
                        Mat CC = SC(Rect(a.x - r, a.y - r, r * 2, r * 2));
                        namedWindow("测量预览图", WINDOW_NORMAL);
                        imshow("测量预览图", CC);
                        waitKey(0);
                        destroyAllWindows();
                    }
                }
            }
        }
        else cout << "    无效指令!" << endl;
        cout << "  ------------------------------------------------------------  " << endl
            << "  请输入您的指令：";
        cin >> _ZL;
    }
    
    return 0;
}
