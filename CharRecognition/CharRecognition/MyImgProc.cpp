#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "MyImgProc.h"

using namespace cv;
using namespace std;

MyImgProc::MyImgProc(){}
MyImgProc::~MyImgProc(){}

//��ǿͼ���еĸ�Ƶ���򣨱�Ե�ͽǵ㣩����ֲ��halcon emphasize������ʵ��Ч����halcon������ϴ�
void MyImgProc::Emphasize(cv::Mat src, cv::Mat dst, int maskwidth, int maskheight, float factor)
{
	blur(src, dst, Size(maskwidth, maskheight));
	subtract(src, dst, dst);
	scaleAdd(dst, factor, src, dst);
}