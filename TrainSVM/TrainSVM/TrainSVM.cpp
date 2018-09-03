// TrainSVM.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <io.h>
#include <string>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\ml.hpp>

using namespace cv;
using namespace ml;
using namespace std;

//�ο��Բ��ģ�https://blog.csdn.net/chaipp0607/article/details/68067098/ 
int _tmain(int argc, _TCHAR* argv[])
{
	////==========================��ȡͼƬ����ѵ������==============================////
	const int imageRows = 16;
	const int imageCols = 8;
	const int classSum = 10;

	Mat classes;
	Mat trainingData;
	Mat trainingImages;
	vector<int> trainingLabels;

	Mat src, resizeImg, trainImg;
	std::cout << "׼������..." << std::endl;
	for (int i = 0; i < classSum; i++)
	{
		//Ŀ���ļ���·��
		//std::string inPath = "D:/ʵϰ/seg/datasets/cvSamplesDigits/";
		std::string inPath = "D:/ʵϰ/seg/TrainImages/";
		char temp[256];
		int k = 0;
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
			//std::string imgname = "D:/ʵϰ/seg/datasets/cvSamplesDigits/";
			std::string imgname = "D:/ʵϰ/seg/TrainImages/";
			imgname = imgname + temp + "/" + fileinfo.name;
			src = imread(imgname, 0);
			if (src.empty())
			{
				std::cout << "can not load image \n" << std::endl;
				return -1;
			}
			resize(src, resizeImg, Size(imageCols, imageRows), (0, 0), (0, 0), INTER_AREA);
			//threshold(resizeImg, resizeImg, 0, 255, THRESH_BINARY | THRESH_OTSU);
			resizeImg = resizeImg.reshape(1, 1);
			trainingImages.push_back(resizeImg);
			// ���ñ�ǩ����
			trainingLabels.push_back(i);
			k++;

		} while (!_findnext(handle, &fileinfo));

		_findclose(handle);
	}

	Mat(trainingImages).copyTo(trainingData);
	trainingData.convertTo(trainingData, CV_32FC1);
	Mat(trainingLabels).copyTo(classes);
	//ѵ�����ݼ���ǩ
	
	////==========================ѵ������==============================////
	std::cout << "����ģ��" << std::endl;
	// Set up SVM's parameters
	Ptr<SVM> svm = SVM::create();
	//Ptr<SVM> svm = StatModel::load<SVM>("../svm.xml");
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR); //LINEAR POLY
	//svm->setKernel(SVM::POLY);
	//svm->setDegree(1.0);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 3000, 1e-6));

	std::cout << "��ʼѵ��" << std::endl;
	// Train the SVM
	svm->train(trainingData, cv::ml::ROW_SAMPLE, classes);
	//svm->trainAuto(trainDataSet);
	std::cout << "ѵ�����" << std::endl;
	svm->save("../svm.xml");
	return 0;
}

