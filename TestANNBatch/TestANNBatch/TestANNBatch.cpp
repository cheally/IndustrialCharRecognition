// TestSVMBatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include <io.h>
#include "opencv2/imgproc.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<string>
#include<windows.h>
#include <opencv2\opencv.hpp>
#include <opencv2\ml.hpp>

using namespace cv;
using namespace std;
using namespace ml;

void SingleCharRecoANN(const Mat &src, int &charIndex, float &confidence, Ptr<ANN_MLP> &model, int resizeWidth, int resizeHeight);

#define HogFeatureDim 60
int _tmain(int argc, _TCHAR* argv[])
{
	Mat src;
	int total = 0, right = 0, timeall = 0;

	Ptr<ANN_MLP> annModel = StatModel::load<ANN_MLP>("../../TrainANN/ann.xml");

	std::cout << "׼������..." << std::endl;
	for (int i = 0; i < 10; i++)
	{
		//Ŀ���ļ���·��
		std::string inPath = "D:/ʵϰ/seg/test/dataset/gray/";
		char temp[256];
		sprintf_s(temp, "%d", i);
		inPath = inPath + temp + "/*.png";
		//���ڲ��ҵľ��
		long long handle;
		struct _finddata_t fileinfo;
		//��һ�β���
		handle = _findfirst(inPath.c_str(), &fileinfo);
		if (handle == -1)
			return -1;
		do
		{
			//�ҵ����ļ����ļ���
			std::string imgname = "D:/ʵϰ/seg/test/dataset/gray/";
			imgname = imgname + temp + "/" + fileinfo.name;
			long t1 = GetTickCount();
			src = imread(imgname, 0);
			if (src.empty())
			{
				std::cout << "can not load image \n" << std::endl;
				return -1;
			}
			int prediction; 
			float conf;
			SingleCharRecoANN(src, prediction, conf, annModel, 8, 16);
			long t2 = GetTickCount();
			timeall += (t2 - t1) * 10;
			if (i == prediction)
				++right;
			++total;
			cout << i << "    :" << prediction << endl;
		} while (!_findnext(handle, &fileinfo));
		_findclose(handle);
	}
	cout << "�ܹ������ַ����� " << total << endl;
	cout << "��ȷ�ַ����� " << right << endl;
	cout << "׼ȷ��Ϊ�� " << right * 100 / total << endl;
	cout << "ƽ����ʱΪ�� " << timeall / total << " us*100" << endl;
	return 0;
}

/* ʶ�𵥸��ַ��� ANN
* @param src �����ַ���ͼ��
* @param charIndex ����ַ�������������
* @param confidence ���Ԥ������Ŷ�
* @param model ANNģ��
* @param resizeWidth ͼ������ߴ�
* @param resizeHeight ͼ������ߴ�
*/
void SingleCharRecoANN(const Mat &src, int &charIndex, float &confidence, Ptr<ANN_MLP> &model, int resizeWidth, int resizeHeight)
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

