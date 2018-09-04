#include "stdafx.h"
#include "MyImgProc.h"
#include<windows.h>

using namespace cv;
using namespace std;
using namespace ml;

MyImgProc::MyImgProc(){}
MyImgProc::~MyImgProc(){}

//��ǿͼ���еĸ�Ƶ���򣨱�Ե�ͽǵ㣩����ֲ��halcon emphasize������ʵ��Ч����halcon������ϴ�
void MyImgProc::Emphasize(const Mat &src, Mat &dst, int maskwidth, int maskheight, float factor)
{
	blur(src, dst, Size(maskwidth, maskheight));
	subtract(src, dst, dst);
	scaleAdd(dst, factor, src, dst);
}

//��̬ѧ����
void MyImgProc::Morphology(const Mat &src, Mat &dst)
{
	Mat recElement = getStructuringElement(MORPH_RECT, Size(30, 9));
	Mat ellElement = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	dilate(src, dst, ellElement);
	//erode(src, dst, DilationElement);
	//dilate(src, dst, DilationElement);
}

//��ȡ�������򡣲ο��ԣ�https://blog.csdn.net/lgh0824/article/details/76100599
/*   ����������������
* @param src ����ͼ��
* @param rRects �洢��Χ�����Ĵ���ת���ε�����
* @param areaMin ���������Сֵ
* @param areaMax ����������ֵ
* @param externalFlag ֻѰ�������������־λ
* @param horizontalRectFlag ֻѰ��ˮƽ���α�־λ
*/
void MyImgProc::FindTextRegion(const Mat &src, vector<RotatedRect> &rRects, int areaMin, int areaMax, bool externalFlag, bool horizontalRectFlag)
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
			rRects.push_back(rect);
		}
		else
		{
			//�ҵ���С���Σ��þ��ο����з���
			RotatedRect rect = minAreaRect(contours[i]);

			//����ߺͿ�
			//int m_width = rect.boundingRect().width;
			//int m_height = rect.boundingRect().height;

			//����������rect��ӵ�rects������
			rRects.push_back(rect);
		}

	}
	//return rects;
}

/*  ������Χ�����ľ���
* @param src ����ͼ��
* @param rRects �洢�˾��ε�����
* @param showFlag ��ʾͼ���־λ
* @param color ָʾ������ɫ������(0,0,0)
*/
void MyImgProc::DrawRects(const Mat &src, vector<RotatedRect> &rRects, bool showFlag, cv::Scalar color)
{
	if (showFlag)
	{
		Mat image;
		src.copyTo(image);
		for each (RotatedRect rect in rRects)
		{
			Point2f P[4];
			rect.points(P);
			for (int j = 0; j <= 3; j++)
			{
				line(image, P[j], P[(j + 1) % 4], color, 1);
			}
		}
		cv::namedWindow("drawPicture", CV_WINDOW_NORMAL);
		imshow("drawPicture", image);
		waitKey(0);
	}
}

/* �Զ����ַ��İ�Χ���ν������򣬴������£���������
* @param rRects ����ľ�������
* @param rects �����ˮƽ��������
* @param row �ַ���������Ŀǰ���֧��2��
*/
void MyImgProc::SortMultiRowRects(vector<RotatedRect> &rRects, Rect* rects, int row)
{
	int center_y = 0;
	
	for (int i = 0; i < (rRects).size(); i++)
	{
		center_y += (rRects)[i].center.y;
	}
	center_y /= (rRects).size();
	int row1Index[CharNumMax], row2Index[CharNumMax];
	int row1Cnt = 0, row2Cnt = 0;
	for (int i = 0; i < (rRects).size(); i++)
	{
		if ((rRects)[i].center.y < center_y)
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
	vector<RotatedRect> row1Rect(CharNumMax), row2Rect(CharNumMax);
	for (int i =0; i <row1Cnt; i++)
	{
		row1Rect[i] = (rRects)[row1Index[i]];
	}
	for (int i = 0; i <row2Cnt; i++)
	{
		row2Rect[i] = (rRects)[row2Index[i]];
	}
	SortSingleRowRects(row1Rect, rects, row1Cnt);
	SortSingleRowRects(row2Rect, rects + row1Cnt, row2Cnt);
}

/* �Ե����ַ��İ�Χ���ν������򣬴�������
* @param rRects ����ľ�������
* @param rects �����ˮƽ��������
* @param num �ַ��ĸ���
*/
void MyImgProc::SortSingleRowRects(vector<RotatedRect> &rRects, Rect* rects, int num)
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
		int topLeft_x = (rRects)[i].center.x - (rRects)[i].size.width / 2;
		int topLeft_y = (rRects)[i].center.y - (rRects)[i].size.height / 2;
		Rect rect_(topLeft_x, topLeft_y, (rRects)[i].size.width, (rRects)[i].size.height);
		*(rects++) = rect_;
	}
}

//�ο����ͣ�https://www.cnblogs.com/willwu/p/6133696.html
/* ��תͼ�񣨱���ԭͼ���������ݣ�
* @param src ����ͼ��
* @param dst ���ͼ��
* @param angle ��ת�Ƕ�
*/
void MyImgProc::RotateImage(const Mat &src, Mat &dst, float angle)
{
	cv::Point2f center(src.cols / 2, src.rows / 2);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1);
	cv::Rect bbox = cv::RotatedRect(center, src.size(), angle).boundingRect();

	rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
	rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;

	cv::warpAffine(src, dst, rot, bbox.size(), 1, 0, 255);
}

/* ������ת���ε���ת�Ƕ�
* @param rRects �����������
* @return ��ת�Ƕȣ���λ���㣩
*/
float MyImgProc::CalculateAngle(const vector<RotatedRect> &rRects)
{
	float angle = rRects[0].angle;
	if (0 < abs(angle) && abs(angle) <= 45)  //��ʱ��
		angle = angle;
	else if (45 < abs(angle) && abs(angle) < 90) //˳ʱ��
		angle = 90 - abs(angle);
	return angle;
}

/* ʶ�𵥸��ַ��� ANN
* @param src �����ַ���ͼ��
* @param charIndex ����ַ�������������
* @param confidence ���Ԥ������Ŷ�
* @param model ANNģ��
* @param resizeWidth ͼ������ߴ�
* @param resizeHeight ͼ������ߴ�
*/
void MyImgProc::SingleCharRecoANN(const Mat &src, int &charIndex, float &confidence, Ptr<ANN_MLP> &model, int resizeWidth, int resizeHeight)
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
	//std::cout << "Ԥ������" << maxLoc.x << " ���Ŷ�:" << maxVal * 100 << "%" << std::endl;
	confidence = maxVal;
	charIndex = maxLoc.x;
}

/* ʶ�����ַ��� ANN
* @param src ���ж���ַ�������ͼ��
* @param charIndexs ����ַ�������������
* @param confidences ���Ԥ������Ŷ�
* @param rRects ��Χ�����ַ��ľ��ε�����
* @param charNum �ַ��ܸ���
* @param modelPath ANNģ������·��
* @param resizeWidth ͼ������ߴ�
* @param resizeHeight ͼ������ߴ�
*/
void MyImgProc::MultiCharRecoANN(const Mat &src, int* charIndexs, float* confidences, Rect* rects, int charNum, string modelPath, int resizeWidth, int resizeHeight)
{
	long t1 = GetTickCount();
	Ptr<ANN_MLP> annModel = StatModel::load<ANN_MLP>(modelPath);
	long t2 = GetTickCount();
	cout << "����ģ��ʱ�䣺" << (t2 - t1) << "ms" << endl;
	for (int i = 0; i < charNum; i++)
	{
		Mat singleCharImg = src(rects[i]);
		SingleCharRecoANN(singleCharImg, *charIndexs, *confidences, annModel), ++charIndexs, ++confidences;
		if (0)
		{
			cv::namedWindow("result", CV_WINDOW_NORMAL);
			imshow("result", singleCharImg);
			waitKey(0);
			destroyWindow("result");
		}
	}
}

/* ʶ�𵥸��ַ��� SVM
* @param src �����ַ���ͼ��
* @param sampleFeatureMat hog��������
* @param hog hog������
* @param prediction Ԥ���ַ����
* @param model SVMģ��
* @param resizeWidth ͼ������ߴ�
* @param resizeHeight ͼ������ߴ�
*/
void MyImgProc::SingleCharRecoSVM(const Mat &src, Mat &sampleFeatureMat, HOGDescriptor &hog, int &prediction, Ptr<SVM> &model, int resizeWidth, int resizeHeight)
{
	Mat dst;
	//������ͼ��ת��Ϊ1*128������
	resize(src, dst, Size(resizeWidth, resizeHeight), (0, 0), (0, 0), INTER_AREA);
	vector<float> descriptors;//HOG����������
	hog.compute(dst, descriptors, Size(4, 4));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)
	for (int i = 0; i < HogFeatureDim; i++)
		sampleFeatureMat.at<float>(0, i) = descriptors[i];

	//Mat p = dst.reshape(1, 1);
	//p.convertTo(p, CV_32FC1);

	float response = model->predict(sampleFeatureMat);

	//cout << "Ԥ������" << response << endl;
	prediction = response;
}

/* ʶ�����ַ��� SVM
* @param src ���������ַ�������ͼ��
* @param predictions Ԥ���ַ����
* @param rects �ַ���Χ���εľ���
* @param charNum �ַ�����
* @param modelPath ģ������·��
* @param resizeWidth ͼ������ߴ�
* @param resizeHeight ͼ������ߴ�
*/
void MyImgProc::MultiCharRecoSVM(const Mat &src, int* predictions, Rect* rects, int charNum, string modelPath, int resizeWidth, int resizeHeight)
{
	Mat sampleFeatureMat;
	//��ⴰ��(64,128),��ߴ�(16,16),�鲽��(8,8),cell�ߴ�(8,8),ֱ��ͼbin����9
	HOGDescriptor hog(Size(8, 16), Size(8, 8), Size(4, 4), Size(4, 4), 5);
	sampleFeatureMat = Mat::zeros(1, HogFeatureDim, CV_32FC1);

	long t1 = GetTickCount();
	Ptr<SVM> svmModel = StatModel::load<SVM>(modelPath);
	long t2 = GetTickCount();
	cout << "����ģ��ʱ�䣺" << (t2 - t1) << "ms" << endl;
	for (int i = 0; i < charNum; i++)
	{
		Mat singleCharImg = src(rects[i]);
		
		SingleCharRecoSVM(singleCharImg, sampleFeatureMat, hog, *predictions, svmModel), ++predictions;
		
		if (0)
		{
			cv::namedWindow("result", CV_WINDOW_NORMAL);
			imshow("result", singleCharImg);
			waitKey(0);
			destroyWindow("result");
		}
	}
}
