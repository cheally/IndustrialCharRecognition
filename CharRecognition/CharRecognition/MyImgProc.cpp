#include "stdafx.h"
#include "MyImgProc.h"

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
void MyImgProc::FindTextRegion(cv::Mat src, std::vector<cv::RotatedRect>* rRects, int areaMin, int areaMax, bool externalFlag, bool horizontalRectFlag)
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
			rRects->push_back(rect);
		}
		else
		{
			//�ҵ���С���Σ��þ��ο����з���
			RotatedRect rect = minAreaRect(contours[i]);

			//����ߺͿ�
			//int m_width = rect.boundingRect().width;
			//int m_height = rect.boundingRect().height;

			//����������rect��ӵ�rects������
			rRects->push_back(rect);
		}

	}
	//return rects;
}

void MyImgProc::DrawRects(cv::Mat* src, std::vector<cv::RotatedRect> rRects, bool showFlag, cv::Scalar color)
{
	if (showFlag)
	{
		for each (RotatedRect rect in rRects)
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

void MyImgProc::SortMultiRowRects(std::vector<cv::RotatedRect> rRects, cv::Rect* rects, int row)
{
	int center_y = 0;
	const int charNumMax = 10;
	for (int i = 0; i < rRects.size(); i++)
	{
		center_y += rRects[i].center.y;
	}
	center_y /= rRects.size();
	int row1Index[charNumMax], row2Index[charNumMax];
	int row1Cnt = 0, row2Cnt = 0;
	for (int i = 0; i < rRects.size(); i++)
	{
		if (rRects[i].center.y < center_y)
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
	vector<RotatedRect> row1Rect(charNumMax), row2Rect(charNumMax);
	for (int i =0; i <row1Cnt; i++)
	{
		row1Rect[i] = rRects[row1Index[i]];
	}
	for (int i = 0; i <row2Cnt; i++)
	{
		row2Rect[i] = rRects[row2Index[i]];
	}
	SortSingleRowRects(row1Rect, rects, row1Cnt);
	SortSingleRowRects(row2Rect, rects + row1Cnt, row2Cnt);
}

void MyImgProc::SortSingleRowRects(std::vector<cv::RotatedRect> rRects, cv::Rect* rects, int num)
{
	int minIdx = 0, minCenter_x = rRects[0].center.x;
	vector<RotatedRect> rectTemp(1);
	for (int j = 0; j < num - 1; j++)
	{
		minCenter_x = rRects[j].center.x;
		minIdx = j;
		for (int i = j; i < num; i++)
		{
			if (minCenter_x>rRects[i].center.x)
			{
				minCenter_x = rRects[i].center.x;
				minIdx = i;
			}
		}
		rectTemp[0] = rRects[j];
		rRects[j] = rRects[minIdx];
		rRects[minIdx] = rectTemp[0];
	}
	for (int i = 0; i < num; i++)
	{
		int topLeft_x = rRects[i].center.x - rRects[i].size.width / 2;
		int topLeft_y = rRects[i].center.y - rRects[i].size.height / 2;
		Rect rect_(topLeft_x, topLeft_y, rRects[i].size.width, rRects[i].size.height);
		*(rects++) = rect_;
	}
}

//�ο����ͣ�https://www.cnblogs.com/willwu/p/6133696.html
void MyImgProc::RotateImage(cv::Mat src, cv::Mat* dst, std::vector<cv::RotatedRect> rRects, float angle)
{
	cv::Point2f center(src.cols / 2, src.rows / 2);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1);
	cv::Rect bbox = cv::RotatedRect(center, src.size(), angle).boundingRect();

	rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
	rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;

	cv::warpAffine(src, *dst, rot, bbox.size(), 1, 0, 255);
}

float MyImgProc::CalculateAngle(std::vector<cv::RotatedRect> rRects)
{
	float angle = rRects[0].angle;
	if (0 < abs(angle) && abs(angle) <= 45)  //��ʱ��
		angle = angle;
	else if (45 < abs(angle) && abs(angle) < 90) //˳ʱ��
		angle = 90 - abs(angle);
	return angle;
}

char MyImgProc::SingleCharReco(cv::Mat src, Ptr<ANN_MLP> model, std::string totalChar, int resizeWidth, int resizeHeight)
{
	Mat dst;
	//������ͼ��ת��Ϊ1*128������
	resize(src, dst, Size(resizeWidth, resizeHeight), (0, 0), (0, 0), INTER_AREA);
	Mat_<float> srcMat(1, resizeWidth*resizeHeight);
	for (int i = 0; i < resizeWidth*resizeHeight; i++)
	{
		srcMat.at<float>(0, i) = (float)dst.at<uchar>(i / resizeWidth, i % resizeWidth);
	}
	
	//ʹ��ѵ���õ�ANNԤ��ͼ��
	model->predict(srcMat, dst);

	//ѡ��
	double maxVal = 0;
	Point maxLoc;
	minMaxLoc(dst, NULL, &maxVal, NULL, &maxLoc);
	//cout << dst << endl;
	std::cout << "Ԥ������" << maxLoc.x << " ���Ŷ�:" << maxVal * 100 << "%" << std::endl;

	return totalChar[maxLoc.x];
}

std::string MyImgProc::MultiCharReco(cv::Mat src, cv::Rect* rects, int charNum, string model, bool showSingleCharflag, int waitTime, std::string totalChar)
{
	Ptr<ANN_MLP> annModel = StatModel::load<ANN_MLP>(model);
	string str;
	for (int i = 0; i < charNum; i++)
	{
		Mat singleCharImg = src(rects[i]);
		str.push_back(SingleCharReco(singleCharImg, annModel, totalChar));
		if (showSingleCharflag)
		{
			cv::namedWindow("result", CV_WINDOW_NORMAL);
			imshow("result", singleCharImg);
			waitKey(waitTime);
			destroyWindow("result");
		}
	}
	return str;
}
