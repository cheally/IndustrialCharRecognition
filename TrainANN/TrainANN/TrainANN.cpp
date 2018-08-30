// TrainANN.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <io.h>
#include <string>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\ml.hpp>

using namespace cv;
using namespace ml;

//�ο��Բ��ģ�https://blog.csdn.net/akadiao/article/details/79236458 
int _tmain(int argc, _TCHAR* argv[])
{
	////==========================��ȡͼƬ����ѵ������==============================////
	const int imageRows = 16;
	const int imageCols = 8;
	const int classSum = 10;
	const int imagesSum = 116;
	float trainingData[classSum*imagesSum][imageRows*imageCols] = { { 0 } };//ÿһ��һ��ѵ������
	float labels[classSum*imagesSum][classSum] = { { 0 } };//ѵ��������ǩ
	Mat src, resizeImg, trainImg;
	std::cout << "׼������..." << std::endl;
	for (int i = 0; i < classSum; i++)
	{
		//Ŀ���ļ���·��
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
			std::string imgname = "D:/ʵϰ/seg/TrainImages/";
			imgname = imgname + temp + "/" + fileinfo.name;
			src = imread(imgname, 0);
			if (src.empty())
			{
				std::cout << "can not load image \n" << std::endl;
				return -1;
			}
			resize(src, resizeImg, Size(imageCols, imageRows), (0, 0), (0, 0), INTER_AREA);
			//��ֵ��
			//threshold(resizeImg, trainImg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

			//imshow("image", trainImg);
			//waitKey(5);
			for (int j = 0; j<imageRows*imageCols; j++)
			{
				trainingData[i*imagesSum + k][j] = (float)resizeImg.data[j];
			}
			// ���ñ�ǩ����
			for (int j = 0; j < classSum; j++)
			{
				if (j == i)
					labels[i*imagesSum + k][j] = 1;
				else
					labels[i*imagesSum + k][j] = 0;
			}
			k++;

		} while (!_findnext(handle, &fileinfo));
		Mat labelsMat(classSum*imagesSum, classSum, CV_32FC1, labels);

		_findclose(handle);
	}
	//ѵ�����ݼ���ǩ
	Mat trainingDataMat(classSum*imagesSum, imageRows*imageCols, CV_32FC1, trainingData);
	Mat labelsMat(classSum*imagesSum, classSum, CV_32FC1, labels);
	//std::cout<<"trainingDataMat: \n"<<trainingDataMat<<"\n"<<std::endl;
	//std::cout<<"labelsMat: \n"<<labelsMat<<"\n"<<std::endl;
	////==========================ѵ������==============================////
	std::cout << "����ģ��" << std::endl;
	Ptr<ANN_MLP>model = ANN_MLP::create();
	//Ptr<ANN_MLP> model = StatModel::load<ANN_MLP>("../bpcharModel.xml");
	Mat layerSizes = (Mat_<int>(1, 3) << imageRows*imageCols, imageRows*imageCols*2, classSum);
	model->setLayerSizes(layerSizes);
	model->setTrainMethod(ANN_MLP::BACKPROP, 0.001, 0.1);
	model->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1.0, 1.0);
	model->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS, 10000, 0.0001));

	Ptr<TrainData> trainData = TrainData::create(trainingDataMat, ROW_SAMPLE, labelsMat);
	std::cout << "��ʼѵ��" << std::endl;
	model->train(trainData);
	model->save("../bpcharModel.xml"); //save classifier
	std::cout << "ѵ�����" << std::endl;
	return 0;
}

