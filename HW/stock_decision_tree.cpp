//
//  main.cpp
//  decisionTree.cpp
//
// Based on OpenCV's cpp sample, tree_engine.cpp
// https://github.com/opencv/opencv/blob/master/samples/cpp/tree_engine.cpp
//
// Stock data pulled from quotemedia
// https://www.quotemedia.com/portal/history?qm_symbol=fb
//
// My features include the following:
// Date, Open, High, Low, Close, VWAP, Volume, % Chg, Change, Trade Val, Total Trades
//
// Data spans Jan 2 to Nov 17
// It appears that at this level of data, no predictions can be made. Perhaps if I
//  increased the amount of data, somethings might change.

#include "opencv2/ml.hpp"
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include <iostream>
#include <fstream>

using namespace cv;
using namespace cv::ml;
using namespace std;

string train_and_print_errs(Ptr<StatModel> model, const Ptr<TrainData>& data)
{
    bool ok = model->train(data);
    if( !ok )
    {
        return "Training failed";
    }
    else
    {
        string results = "";
        results.append("Train error: ");
        results.append(to_string(model->calcError(data, false, noArray())));
        results.append("\nTest error: ");
        results.append(to_string(model->calcError(data, true, noArray())));
        return results;
    }
    return "Something went wrong!";
}

string stock_training(string filename)
{
    Ptr<TrainData> data = TrainData::loadFromCSV(filename, 0);
    string results = "";
    if( data.empty() )
    {
        results.append("ERROR: File ");
        results.append(filename.c_str());
        results.append(" can not be read\n");
        return results;
    }
    // Ratio
    data->setTrainTestSplitRatio(0.8);
    results.append("Test/Train: ");
    results.append(to_string(data->getNTestSamples()));
    results.append("/");
    results.append(to_string(data->getNTrainSamples()));
    // Results
    Ptr<DTrees> dtree = DTrees::create();
    dtree->setMaxDepth(10);
    dtree->setMinSampleCount(2);
    dtree->setRegressionAccuracy(0);
    dtree->setUseSurrogates(false);
    dtree->setMaxCategories(16);
    dtree->setCVFolds(0);
    dtree->setUse1SERule(false);
    dtree->setTruncatePrunedTree(false);
    dtree->setPriors(Mat());
    results.append("\n");
    results.append(train_and_print_errs(dtree, data));
    return results;
}

int main()
{
    string filenames[] = {"Amazon.csv","Antero_Resources.csv","Apple.csv","Cisco.csv","CME.csv","Crown_Castle.csv","Ebay.csv","Facebook.csv","Google.csv","HP.csv","IBM.csv","Intel.csv","Marathon_Petroleum.csv","Medpace_Holdings.csv","Micron_Technology.csv","Microsoft.csv","Oracle.csv","Palo_Alto_Networks.csv","Qualcomm.csv","Raytheon.csv","Royal_Gold.csv","Texas_Instruments.csv","Thermo_Fisher_Scientific.csv","Western_Digital.csv","Xerox.csv"};

    for(auto stock: filenames)
    {
        // Header
        cout << "\n======" << stock << "=====\n" << endl;
        cout << stock_training(stock) << endl << endl;
    }
    return 0;
}
















