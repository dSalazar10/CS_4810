//
//  main.cpp
//  hw5
//
//  Based on OpenCV example neural_network.cpp
//  https://github.com/opencv/opencv/blob/master/samples/cpp/neural_network.cpp
//
//  Reused previous HW for comparision in one file
//
//  I used the convert method from opencv forums:
//  http://answers.opencv.org/question/120541/how-do-i-load-and-use-traindata/

#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;

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

string decision_tree(string filename)
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
string ann(string filename)
{
    // Parse the CSV file into a 2D vector of doubles
    ifstream fin(filename);
    string line;
    vector< vector<double> > all_data;
    while(getline(fin, line)){
        vector<double> values;
        stringstream temp(line);
        string single_value;
        while(getline(temp,single_value,',')){
            values.push_back(stod(single_value.c_str()));
        }
        all_data.push_back(values);
    }
    
    // Put the data from vector into a CV Matrix
    Mat data((int)all_data.size(),(int)all_data[0].size(),CV_32F,&all_data.front());
    // Create an empty CV Matrix to represent no responses
    Mat responses(data.rows, 10, CV_32F, cv::Scalar(0));
    
    /*
    // 2
    Mat_<int> layerSizes(1, 2); // modify the number of layers here
    layerSizes(0, 0) = data.cols;
    layerSizes(0, 1) = responses.cols;
    */
    /*
    // 3
    Mat_<int> layerSizes(1, 3); // modify the number of layers here
    layerSizes(0, 0) = data.cols;
    layerSizes(0, 1) = 10;
    layerSizes(0, 2) = responses.cols;
     */
    /*
     // Normal # of nodes is 10, double is 20
    // 4
    Mat_<int> layerSizes(1, 4); // modify the number of layers here
    layerSizes(0, 0) = data.cols;
    layerSizes(0, 1) = 20;
    layerSizes(0, 2) = 20;
    layerSizes(0, 3) = responses.cols;
    */
    
    // 5
    Mat_<int> layerSizes(1, 5); // modify the number of layers here
    layerSizes(0, 0) = data.cols;
    layerSizes(0, 1) = 10;
    layerSizes(0, 2) = 10;
    layerSizes(0, 3) = 10;
    layerSizes(0, 4) = responses.cols;
     
    /*
    // 6
    Mat_<int> layerSizes(1, 6); // modify the number of layers here
    layerSizes(0, 0) = data.cols;
    layerSizes(0, 1) = 10;
    layerSizes(0, 2) = 10;
    layerSizes(0, 3) = 10;
    layerSizes(0, 4) = 10;
    layerSizes(0, 5) = responses.cols;
    */
    
    Ptr<ANN_MLP> network = ANN_MLP::create();
    network->setLayerSizes(layerSizes);
    // Try out other functions besides the sygmoid function
    network->setActivationFunction(ANN_MLP::SIGMOID_SYM, 0.1, 0.1);
    
    network->setTrainMethod(ANN_MLP::BACKPROP, 0, 0);
    Ptr<TrainData> tData = TrainData::create(data, ROW_SAMPLE, responses);
    
    network->train(tData);
    if (network->isTrained())
    {
        string results = "";
        results.append("Train error: ");
        results.append(to_string(network->calcError(tData, false, noArray())));
        results.append("\nTest error: ");
        results.append(to_string(network->calcError(tData, true, noArray())));
        return results;
    }
    return "failed!";
}
std::ofstream& getFile(std::ofstream& fout)
{
    do {
        // Prompt user for a file name
        std::string filename = "";
        std::cout << "Please input the filename: " << std::flush;
        std::cin >> filename;
        try {
            // open file for input
            fout.open(filename, std::ifstream::in);
        }
        catch (std::ofstream::failure e) {
            std::cerr << "Failed to open " << filename << ".\n";
        }
        std::cin.ignore();
    } while (!fout.is_open());
    return fout;
}
int main()
{
    string filenames[] = {"Amazon.csv","Antero_Resources.csv","Apple.csv","Cisco.csv","CME.csv","Crown_Castle.csv","Ebay.csv","Facebook.csv","Google.csv","HP.csv","IBM.csv","Intel.csv","Marathon_Petroleum.csv","Medpace_Holdings.csv","Micron_Technology.csv","Microsoft.csv","Oracle.csv","Palo_Alto_Networks.csv","Qualcomm.csv","Raytheon.csv","Royal_Gold.csv","Texas_Instruments.csv","Thermo_Fisher_Scientific.csv","Western_Digital.csv","Xerox.csv"};
    ofstream fout;
    getFile(fout);
    for(auto stock: filenames)
    {
        // Header
        fout << "\n======" << stock << "=====\n" << endl;
        fout << "Decision Tree:\n";
        fout << decision_tree(stock) << endl << endl;
        fout << "Neural Network:\n";
        fout << ann(stock) << endl;
    }
    return 0;
}












