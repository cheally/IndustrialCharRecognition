// CharRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Industrial Character Recognition" << endl;
	Mat SrcImg = imread("D:/ʵϰ/ͼƬ/pic/2017.08.25/C1-08251718/C1_0.bmp", 0);
	if (SrcImg.empty())
	{
		std::cout << "��ͼƬʧ��,����" << std::endl;
		return -1;
	}
	imshow("img", SrcImg);
	waitKey(0);

	return 0;
}

