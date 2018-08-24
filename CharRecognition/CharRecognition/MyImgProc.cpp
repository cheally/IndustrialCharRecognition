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
void MyImgProc::Emphasize(cv::Mat src, cv::Mat* dst, int maskwidth, int maskheight, float factor)
{
	blur(src, *dst, Size(maskwidth, maskheight));
	subtract(src, *dst, *dst);
	scaleAdd(*dst, factor, src, *dst);
}

//��̬ѧ����
void MyImgProc::Morphology(cv::Mat src, cv::Mat* dst)
{
	Mat recElement = getStructuringElement(MORPH_RECT, Size(30, 9));
	Mat ellationElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(src, *dst, ellationElement);
	//erode(*src, *dst, DilationElement);
	//dilate(*src, *dst, DilationElement);
}

//��ȡ�������򡣲ο��ԣ�https://blog.csdn.net/lgh0824/article/details/76100599
void MyImgProc::FindTextRegion(cv::Mat src, std::vector<cv::RotatedRect>* rects)
{
	//vector<RotatedRect> rects;
	//1.��������
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(src, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//2.ɸѡ��Щ���С��
	for (int i = 0; i < contours.size(); i++)
	{
		//���㵱ǰ���������
		double area = contourArea(contours[i]);

		//���С��1000��ȫ��ɸѡ��
		if (area < 3600 || area > 25000)
			continue;

		//�������ƣ����ý�С��approxPolyDP�����д��о�
		double epsilon = 0.001*arcLength(contours[i], true);
		Mat approx;
		approxPolyDP(contours[i], approx, epsilon, true);

		//�ҵ���С���Σ��þ��ο����з���
		RotatedRect rect = minAreaRect(contours[i]);

		//����ߺͿ�
		int m_width = rect.boundingRect().width;
		int m_height = rect.boundingRect().height;

		//ɸѡ��Щ̫ϸ�ľ��Σ����±��
		if (m_height > m_width * 1.2)
			continue;

		//����������rect��ӵ�rects������
		rects->push_back(rect);

	}
	//return rects;
}