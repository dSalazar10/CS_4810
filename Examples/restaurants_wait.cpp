// from https://github.com/opencv/opencv/blob/master/samples/cpp/points_classifier.cpp
#include "opencv2/core.hpp"
#include "opencv2/ml.hpp"

#include <stdio.h>

using namespace std;
using namespace cv;
using namespace cv::ml;

// load training data from a CSV file
static Ptr<TrainData> prepare_train_data()
{
	Ptr<TrainData> tdata = TrainData::loadFromCSV("data.txt", 0, -1, -1,
		"cat[0-6]");

	Mat data = tdata->getTrainSamples();
	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < 6; col++)
			cout << data.at<float>(row, col) << " ";
		cout << endl;
	}

	return tdata;
}

// generate data to test model and run the classifier
static void predict_and_paint(const Ptr<StatModel>& model)
{
	Mat testSample(1, 6, CV_32FC1);
	// Using output from above --
	// values for categorical data would change
	// if the input data changes
	testSample.at<float>(0) = (float)1;
	testSample.at<float>(1) = (float)0;
	testSample.at<float>(2) = (float)0;
	testSample.at<float>(3) = (float)1;
	testSample.at<float>(4) = (float)3; // full
	testSample.at<float>(5) = (float)4; // $

	int response = (int)model->predict(testSample);
	// 1 for wait, 0 for leave
	cout << "response = " << response << endl;
}

// generate decision tree model and try it
static void find_decision_boundary_DT()
{
	Ptr<DTrees> dtree = DTrees::create();
	dtree->setMaxDepth(8);
	dtree->setMinSampleCount(1);
	dtree->setUseSurrogates(false);
	dtree->setCVFolds(0); // the number of cross-validation folds
	dtree->setUse1SERule(false);
	dtree->setTruncatePrunedTree(false);
	dtree->train(prepare_train_data());
	predict_and_paint(dtree);
}

int main()
{
	find_decision_boundary_DT();

	system("pause");

	return 0;
}