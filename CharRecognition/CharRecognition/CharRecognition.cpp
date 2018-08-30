//-----------------------------------������˵����----------------------------------------------
//		��������:��������opencv��C++�Ĺ�ҵ�ַ���⡷ ��������Դ�� 
//		��������IDE�汾��Visual Studio 2013
//		��������OpenCV�汾��	3.4
//		2018��8��18�� Created by zhuwei
//		github��ַ��https://github.com/weiSupreme/IndustrialCharRecognition.git  
//		���ͣ�https://weiSupreme.github.io 
//----------------------------------------------------------------------------------------------
 
// CharRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "MyImgProc.h"
#include<string>
#include<windows.h>
#include <opencv2\ml.hpp>

using namespace cv;
using namespace std;
using namespace ml;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Industrial Character Recognition" << endl;
	//string imgName = "images/C1_12 (2).bmp";
	string imgName = "D:/ʵϰ/ͼƬ/pic/2017.08.25/C1-08251718/C1_128 (2).bmp";
	Mat srcImg = imread(imgName, 0);
	if (srcImg.empty())
	{
		std::cout << "��ͼƬʧ��,���顣ͼƬ����� ��IndustrialCharRecognition\CharRecognition\CharRecognition\images�� " << std::endl;
		return -1;
	}

	long t1 = GetTickCount();

	MyImgProc *mip = new MyImgProc();

	//Ԥ�����˲�����ֵ�ָ�
	Mat emphasizeImg;
	//mip->Emphasize(srcImg, &emphasizeImg, 7, 7, 5);
	medianBlur(srcImg, emphasizeImg, 3);
	Mat binaryImg;
	threshold(emphasizeImg, binaryImg, 200, 255, 1);
	
	//��̬ѧ����
	Mat morphologyImg;
	Mat ellElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(binaryImg, morphologyImg, ellElement);

	//Ѱ����������
	vector<RotatedRect> rotatedRects;
	mip->FindTextRegion(morphologyImg, &rotatedRects, 3600, 25000);
	if (rotatedRects.size() != 1)
	{
		cout << "�ҵ�" << rotatedRects.size() << "���ı�����" << endl;
		return 0;
	}

	//��������
	mip->DrawRects(&srcImg, rotatedRects, false);

	//�����ı�������ת�Ƕ�
	float angle=0.0;
	angle = mip->CalculateAngle(rotatedRects);

	//��ȡˮƽ����
	Rect horizontalRect = rotatedRects[0].boundingRect();
	Mat reducedGrayImg = srcImg(horizontalRect);

	//��תͼ�񣬵õ�ˮƽ����
	Mat rotatedGrayImg;
	mip->RotateImage(reducedGrayImg, &rotatedGrayImg, rotatedRects, angle);

	//��ֵ����
	Mat rotatedBinaryImg;
	threshold(rotatedGrayImg, rotatedBinaryImg, 190, 255, 1);
	
	//��̬ѧ����
	Mat morphologyImgRotated;
	Mat ellElement2 = getStructuringElement(MORPH_RECT, Size(1, 2));
	dilate(rotatedBinaryImg, morphologyImgRotated, ellElement2);

	//�ָ���ַ�
	vector<RotatedRect> rotatedRectsChar;
	mip->FindTextRegion(morphologyImgRotated, &rotatedRectsChar, 10, 600, true, true);

	mip->DrawRects(&rotatedGrayImg, rotatedRectsChar, true, Scalar(127, 127, 127));

	//�����ʶ��
	const int charNum = 15;
	int index[charNum];
	float conf[charNum];
	Rect sortedRectsChar[charNum];
	mip->SortMultiRowRects(rotatedRectsChar, sortedRectsChar);
	mip->MultiCharReco(rotatedGrayImg, index, conf, sortedRectsChar, rotatedRectsChar.size(), "../../TrainAnn/bpcharModel.xml");
	
	long t2 = GetTickCount();
	cout << "����ʱ�䣺" << (t2 - t1 - 78) << "ms" << endl;
	string totalChar = "0123456789";
	for (byte i = 0; i < charNum; i++)
	{
		cout << "ʶ����ַ�Ϊ��" << totalChar[index[i]]<<"   ";
		cout << "���Ŷ�Ϊ��" << conf[i] << endl;
	}

	return 0;
}

