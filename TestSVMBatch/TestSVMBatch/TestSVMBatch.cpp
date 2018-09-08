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

void SingleCharRecoSVM(const Mat &src, Mat &sampleFeatureMat, HOGDescriptor &hog, int &prediction, Ptr<SVM> &model, int resizeWidth, int resizeHeight);

#define HogFeatureDim 60
int _tmain(int argc, _TCHAR* argv[])
{
	Mat src;
	int total = 0, right = 0, timeall = 0;
	Mat sampleFeatureMat;
	//��ⴰ��(64,128),��ߴ�(16,16),�鲽��(8,8),cell�ߴ�(8,8),ֱ��ͼbin����9
	HOGDescriptor hog(Size(8, 16), Size(8, 8), Size(4, 4), Size(4, 4), 5);
	sampleFeatureMat = Mat::zeros(1, HogFeatureDim, CV_32FC1);

	Ptr<SVM> svmModel = StatModel::load<SVM>("../../TrainSVM/svm.xml");

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
			SingleCharRecoSVM(src, sampleFeatureMat, hog, prediction, svmModel, 8, 16);
			long t2 = GetTickCount();
			timeall += (t2 - t1);
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
	cout << "ƽ����ʱΪ�� " << timeall / total << " ms" << endl;
	return 0;
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
void SingleCharRecoSVM(const Mat &src, Mat &sampleFeatureMat, HOGDescriptor &hog, int &prediction, Ptr<SVM> &model, int resizeWidth, int resizeHeight)
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

