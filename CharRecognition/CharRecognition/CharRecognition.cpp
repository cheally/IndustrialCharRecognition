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
	cout << "*********Industrial Character Recognition********" << endl;
	cout << "���Գ����Դ�ͼƬ�������� 0 " << endl;
	cout << "ѡ����������ͼƬ�� ������ 1" << endl;
	char chKeyB=0;
	while (!chKeyB)
	{
		cin >> chKeyB;
	}
	string imgName;
	if (chKeyB == '0')
	{
		imgName = "images/C1_215.bmp";
		//imgName = "D:/ʵϰ/ͼƬ/pic/2017.08.25/C1-08251718/C1_125 (2).bmp";
		cout << "ͼƬ·��Ϊ�� " << imgName << endl;
	}
	else if (chKeyB == '1')
	{
		cout << "������ͼƬ·����"<< endl;
		cin >> imgName;
		cout << "ͼƬ·��Ϊ�� " << imgName << endl;
	}
	else
	{
		cout << "��ѡ����ȷ�Ĳ���" << endl;
	}
	
	Mat srcImg = imread(imgName, 0);
	if (srcImg.empty())
	{
		std::cout << "��ͼƬʧ�ܣ�����" << std::endl;
		return -1;
	}

	long t1 = GetTickCount();

	MyImgProc *mip = new MyImgProc();

	//Ԥ�����˲�����ֵ�ָ�
	Mat emphasizeImg;
	medianBlur(srcImg, emphasizeImg, 3);
	Mat binaryImg;
	threshold(emphasizeImg, binaryImg, 200, 255, 1);

	//��̬ѧ����
	Mat morphologyImg;
	Mat ellElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(binaryImg, morphologyImg, ellElement);
	//����16ms

	//Ѱ����������
	vector<RotatedRect> rotatedRects;
	mip->FindTextRegion(morphologyImg, rotatedRects, 3600, 25000);
	if (rotatedRects.size() != 1)
	{
		cout << "�ҵ�" << rotatedRects.size() << "���ı�����" << endl;
		return 0;
	}
	//����32ms

	//��������
	mip->DrawRects(srcImg, rotatedRects, false);

	//�����ı�������ת�Ƕ�
	float angle=0.0;
	angle = mip->CalculateAngle(rotatedRects);

	//��ȡˮƽ����
	Rect horizontalRect = rotatedRects[0].boundingRect();
	Mat reducedGrayImg = srcImg(horizontalRect);

	//��תͼ�񣬵õ�ˮƽ����
	Mat rotatedGrayImg;
	mip->RotateImage(reducedGrayImg, rotatedGrayImg, angle);

	//��ֵ����
	Mat rotatedBinaryImg;
	threshold(rotatedGrayImg, rotatedBinaryImg, 190, 255, 1);

	//��̬ѧ����
	Mat morphologyImgRotated;
	Mat ellElement2 = getStructuringElement(MORPH_RECT, Size(1, 2));
	dilate(rotatedBinaryImg, morphologyImgRotated, ellElement2);

	//�ָ���ַ�
	vector<RotatedRect> rotatedRectsChar;
	mip->FindTextRegion(morphologyImgRotated, rotatedRectsChar, 30, 600, true, true);

	mip->DrawRects(rotatedGrayImg, rotatedRectsChar, false, Scalar(127, 127, 127));

	//�����ʶ��
	const int charNum = 15;
	Rect sortedRectsChar[charNum];
	mip->SortMultiRowRects(rotatedRectsChar, sortedRectsChar);

	long t2 = GetTickCount();
	cout << "ͼ����ʱ�䣺" << (t2 - t1) << "ms" << endl;

	if (0)  //ANN
	{
		cout << "Using ANN" << endl;
		int index[charNum];
		float conf[charNum];
		mip->MultiCharRecoANN(rotatedGrayImg, index, conf, sortedRectsChar, rotatedRectsChar.size(), "../../TrainANN/ann.xml", false, 0);
		long t3 = GetTickCount();
		cout << "ʶ��ʱ�䣺" << (t3 - t2) << "ms" << endl;
		string totalChar = "0123456789";
		for (byte i = 0; i < charNum; i++)
		{
			cout << "ʶ����ַ�Ϊ��" << totalChar[index[i]] << "   ";
			cout << "���Ŷ�Ϊ��" << conf[i] << endl;
		}
	}
	else //SVM
	{
		cout << "Using SVM" << endl;
		int results[charNum];
		mip->MultiCharRecoSVM(rotatedGrayImg, results, sortedRectsChar, rotatedRectsChar.size(), "../../TrainSVM/svm.xml", false, 0);
		long t3 = GetTickCount();
		cout << "ʶ��ʱ�䣺" << (t3 - t2) << "ms" << endl;
		for (byte i = 0; i < charNum; i++)
		{
			cout << "ʶ����ַ�Ϊ��" << results[i] << endl;
		}
	}

	mip->DrawRects(rotatedGrayImg, rotatedRectsChar, true, Scalar(127, 127, 127));
	
	//cv::namedWindow("rotatedGrayImg", CV_WINDOW_NORMAL);
	//imshow("rotatedGrayImg", rotatedGrayImg);
	//waitKey(0);

	return 0;
}

