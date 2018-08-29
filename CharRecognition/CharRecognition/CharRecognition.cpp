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

using namespace cv;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Industrial Character Recognition" << endl;
	Mat srcImg = imread("images/C1_12 (2).bmp", 0);
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
	Mat ellElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(binaryImg, morphologyImg, ellElement);

	//Ѱ����������
	vector<RotatedRect> rects;
	mip->FindTextRegion(morphologyImg, &rects, 3600, 25000);

	//��������
	mip->DrawRects(&srcImg, rects, false);

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
		angle = mip->CalculateAngle(rects);
	}

	//��תͼ�񣬵õ�ˮƽ����
	Mat rotatedImg;
	Mat M = getRotationMatrix2D(rects[0].center, angle, 1);
	warpAffine(srcImg, rotatedImg, M, srcImg.size());

	//��ֵ�ָ�
	Mat binaryImgRotated;
	threshold(rotatedImg, binaryImgRotated, 180, 255, 1);

	//��̬ѧ����
	Mat morphologyImgRotated;
	Mat ellElement2 = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(binaryImgRotated, morphologyImgRotated, ellElement2);

	//Ѱ���ַ�����
	vector<RotatedRect> rectsCharRegion;
	mip->FindTextRegion(morphologyImgRotated, &rectsCharRegion, 3600, 8000, false, true);

	//��������
	mip->DrawRects(&rotatedImg, rectsCharRegion, false);

	//��С�ַ�����
	int topLeft_x = rectsCharRegion[0].center.x - rectsCharRegion[0].size.width / 2;
	int topLeft_y = rectsCharRegion[0].center.y - rectsCharRegion[0].size.height / 2;
	Rect CharRegionRect(topLeft_x, topLeft_y, rectsCharRegion[0].size.width, rectsCharRegion[0].size.height);
	Mat reducedBinaryImg = binaryImgRotated(CharRegionRect);
	//Mat reducedGrayImg = rotatedImg(CharRegionRect);
	//cout << CharRegionRect << endl;

	//�ָ���ַ�
	vector<RotatedRect> charRects;
	mip->FindTextRegion(reducedBinaryImg, &charRects, 30, 300, true, true);

	mip->DrawRects(&reducedBinaryImg, charRects, false, Scalar(127, 127, 127));

	Rect sortedcharRects[15];
	mip->SortMultiRowRects(charRects, sortedcharRects);

	//���ַ�ʶ��
	string recoStr = "201808253621718";
	for (int i = 0; i < charRects.size(); i++)
	{
		Mat singleCharImg = reducedBinaryImg(sortedcharRects[i]);
		recoStr[i] = mip->SingleCharReco(singleCharImg, "../bpcharModel.xml");
		cv::namedWindow("result", CV_WINDOW_NORMAL);
		imshow("result", singleCharImg);
		waitKey(200);
		destroyWindow("result");
	}

	cout << "ʶ����ַ�Ϊ��" << recoStr << endl;

	//cv::namedWindow("result", CV_WINDOW_NORMAL);
	//imshow("result", reducedBinaryImg);
	//imwrite("D:/ʵϰ/ͼƬ/pic/2017.08.25/C1-08251718/a1_0.bmp", DstImg);
	//waitKey(0);

	return 0;
}

