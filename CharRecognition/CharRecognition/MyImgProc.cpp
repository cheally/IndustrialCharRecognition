#include "stdafx.h"
#include<iostream>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "MyImgProc.h"
#include <opencv2\ml.hpp>

using namespace cv;
using namespace std;
using namespace ml;

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
	Mat ellElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(src, *dst, ellElement);
	//erode(*src, *dst, DilationElement);
	//dilate(*src, *dst, DilationElement);
}

//��ȡ�������򡣲ο��ԣ�https://blog.csdn.net/lgh0824/article/details/76100599
void MyImgProc::FindTextRegion(cv::Mat src, std::vector<cv::RotatedRect>* rects, int areaMin, int areaMax, bool externalFlag, bool horizontalRectFlag)
{
	//1.��������
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	if (externalFlag)
	{
		findContours(src, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	}
	else
	{
		findContours(src, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));
	}

	//2.ɸѡ��Щ���С��
	for (int i = 0; i < contours.size(); i++)
	{
		//���㵱ǰ���������
		double area = contourArea(contours[i]);

		//���С��1000��ȫ��ɸѡ��
		if (area < areaMin || area > areaMax)
			continue;

		//�������ƣ����ý�С��approxPolyDP�����д��о�
		double epsilon = 0.001*arcLength(contours[i], true);
		Mat approx;
		approxPolyDP(contours[i], approx, epsilon, true);

		if (horizontalRectFlag)
		{
			Rect rectHoriz = boundingRect(contours[i]);
			float center_x = rectHoriz.x + rectHoriz.width / 2;
			float center_y = rectHoriz.y + rectHoriz.height / 2;
			RotatedRect rect(Point2f(center_x, center_y), Size2f(rectHoriz.width, rectHoriz.height), 0);
			rects->push_back(rect);
		}
		else
		{
			//�ҵ���С���Σ��þ��ο����з���
			RotatedRect rect = minAreaRect(contours[i]);

			//����ߺͿ�
			//int m_width = rect.boundingRect().width;
			//int m_height = rect.boundingRect().height;

			//����������rect��ӵ�rects������
			rects->push_back(rect);
		}

	}
	//return rects;
}

void MyImgProc::DrawRects(cv::Mat* src, std::vector<cv::RotatedRect> rects, bool showFlag, cv::Scalar color)
{
	if (showFlag)
	{
		for each (RotatedRect rect in rects)
		{
			Point2f P[4];
			rect.points(P);
			for (int j = 0; j <= 3; j++)
			{
				line(*src, P[j], P[(j + 1) % 4], color, 1);
			}
		}
	}
}

void MyImgProc::SortMultiRowRects(std::vector<cv::RotatedRect> rotatedRects, cv::Rect* rect, int row)
{
	int center_y = 0;
	for (int i = 0; i < rotatedRects.size(); i++)
	{
		center_y += rotatedRects[i].center.y;
	}
	center_y /= rotatedRects.size();
	int row1Index[8], row2Index[7];
	int row1Cnt = 0, row2Cnt = 0;
	for (int i = 0; i < rotatedRects.size(); i++)
	{
		if (rotatedRects[i].center.y < center_y)
		{
			row1Index[row1Cnt] = i;
			row1Cnt++;
		}
		else
		{
			row2Index[row2Cnt] = i;
			row2Cnt++;
		}
	}
	vector<RotatedRect> row1Rect(8), row2Rect(7);
	for (int i =0; i <row1Cnt; i++)
	{
		row1Rect[i] = rotatedRects[row1Index[i]];
	}
	for (int i = 0; i <row2Cnt; i++)
	{
		row2Rect[i] = rotatedRects[row2Index[i]];
	}
	SortSingleRowRects(row1Rect, rect);
	SortSingleRowRects(row2Rect, rect + row1Cnt);
}

void MyImgProc::SortSingleRowRects(std::vector<cv::RotatedRect> rotatedRects, cv::Rect* rect)
{
	int minIdx = 0, minCenter_x = rotatedRects[0].center.x;
	vector<RotatedRect> rectTemp(1);
	for (int j = 0; j < rotatedRects.size() - 1; j++)
	{
		minCenter_x = rotatedRects[j].center.x;
		minIdx = j;
		for (int i = j; i < rotatedRects.size(); i++)
		{
			if (minCenter_x>rotatedRects[i].center.x)
			{
				minCenter_x = rotatedRects[i].center.x;
				minIdx = i;
			}
		}
		rectTemp[0] = rotatedRects[j];
		rotatedRects[j] = rotatedRects[minIdx];
		rotatedRects[minIdx] = rectTemp[0];
	}
	for (int i = 0; i < rotatedRects.size(); i++)
	{
		int topLeft_x = rotatedRects[i].center.x - rotatedRects[i].size.width / 2;
		int topLeft_y = rotatedRects[i].center.y - rotatedRects[i].size.height / 2;
		Rect rect_(topLeft_x, topLeft_y, rotatedRects[i].size.width, rotatedRects[i].size.height);
		*(rect++) = rect_;
	}
}

float MyImgProc::CalculateAngle(std::vector<cv::RotatedRect> rect)
{
	float angle = rect[0].angle;
	if (0 < abs(angle) && abs(angle) <= 45)  //��ʱ��
		angle = angle;
	else if (45 < abs(angle) && abs(angle) < 90) //˳ʱ��
		angle = 90 - abs(angle);
	return angle;
}

char MyImgProc::SingleCharReco(cv::Mat src, std::string filePath, std::string totalChar, int resizeWidth, int resizeHeight)
{
	//��ȡģ��
	Ptr<ANN_MLP> model = StatModel::load<ANN_MLP>(filePath);

	Mat dst;
	//������ͼ��ת��Ϊ1*128������
	resize(src, dst, Size(resizeWidth, resizeHeight), (0, 0), (0, 0), INTER_AREA);
	Mat_<float> srcMat(1, resizeWidth*resizeHeight);
	for (int i = 0; i < resizeWidth*resizeHeight; i++)
	{
		srcMat.at<float>(0, i) = (float)dst.at<uchar>(i / 8, i % 8);
	}
	
	//ʹ��ѵ���õ�MLP modelԤ�����ͼ��
	model->predict(srcMat, dst);

	//ѡ��
	double maxVal = 0;
	Point maxLoc;
	minMaxLoc(dst, NULL, &maxVal, NULL, &maxLoc);
	std::cout << "Ԥ������" << maxLoc.x << " ���Ŷ�:" << maxVal * 100 << "%" << std::endl;

	return totalChar[maxLoc.x];
}