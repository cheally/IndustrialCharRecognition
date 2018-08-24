// CharRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "MyImgProc.h"

using namespace cv;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Industrial Character Recognition" << endl;
	Mat srcImg = imread("D:/ʵϰ/ͼƬ/pic/2017.08.25/C1-08251718/C1_12 (2).bmp", 0);
	if (srcImg.empty())
	{
		std::cout << "��ͼƬʧ��,����" << std::endl;
		return -1;
	}
	
	MyImgProc *mip = new MyImgProc();

	//Ԥ�����˲�����ֵ�ָ�
	Mat emphasizeImg;
	mip->Emphasize(srcImg, &emphasizeImg, 7, 7, 5);
	Mat binaryImg;
	threshold(emphasizeImg, binaryImg, 190, 255, 1);

	//��̬ѧ����
	Mat morphologyImg;
	mip->Morphology(binaryImg, &morphologyImg);

	//Ѱ����������
	vector<RotatedRect> rects;
	mip->FindTextRegion(morphologyImg, &rects);

	//��������
	if (0)
	{
		for each (RotatedRect rect in rects)
		{
			Point2f P[4];
			rect.points(P);
			for (int j = 0; j <= 3; j++)
			{
				line(srcImg, P[j], P[(j + 1) % 4], Scalar(0, 255, 0), 2);
			}
		}
	}

	//�����ı�������ת�Ƕ�
	//cout << rects.size() << endl;
	float angle=0.0;
	if (rects.size() != 1)
	{
		cout << "�ҵ�"<<rects.size()<<"���ı�����" << endl;
		return 0;
	}
	else
	{
		angle = rects[0].angle;
		if (0 < abs(angle) && abs(angle) <= 45)  //��ʱ��
			angle = angle;
		else if (45 < abs(angle) && abs(angle) < 90) //˳ʱ��
			angle = 90 - abs(angle);
	}

	//��תͼ�񣬵õ�ˮƽ����
	Mat rotatedImg;
	Mat M = getRotationMatrix2D(rects[0].center, angle, 1);
	warpAffine(srcImg, rotatedImg, M, srcImg.size());

	imshow("img", rotatedImg);
	//imwrite("D:/ʵϰ/ͼƬ/pic/2017.08.25/C1-08251718/a1_0.bmp", DstImg);
	waitKey(0);

	return 0;
}

