#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#include "structIO.hpp"

using namespace std;

void matchDescriptors(cv::Mat &imgSource, cv::Mat &imgRef, vector<cv::KeyPoint> &kPtsSource, vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource, cv::Mat &descRef,
                      vector<cv::DMatch> &matches, string descriptorType, string matcherType, string selectorType)
{

    // configure matcher
    bool crossCheck = true;
    cv::Ptr<cv::DescriptorMatcher> matcher;

    if (matcherType.compare("MAT_BF") == 0)
    {
        int normType = descriptorType.compare("DES_BINARY") == 0 ? cv::NORM_HAMMING : cv::NORM_L2;
        matcher = cv::BFMatcher::create(normType, crossCheck);
        cout << "BF matching cross-check=" << crossCheck;
    }
    else if (matcherType.compare("MAT_FLANN") == 0)
    {
        if (descSource.type() != CV_32F)
        { // OpenCV bug workaround : convert binary descriptors to floating point due to a bug in current OpenCV implementation
            descSource.convertTo(descSource, CV_32F);
            descRef.convertTo(descRef, CV_32F);
        }

        //... TODO : implement FLANN matching
        matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
        cout << "FLANN matching";
    }

    // perform matching task
    if (selectorType.compare("SEL_NN") == 0)
    { // nearest neighbor (best match)

        double t = (double)cv::getTickCount();
        matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << " (NN) with n=" << matches.size() << " matches in " << 1000 * t / 1.0 << " ms" << endl;
    }
    else if (selectorType.compare("SEL_KNN") == 0)
    { // k nearest neighbors (k=2)
        // TODO : implement k-nearest-neighbor matching
        int k = 2;
        double t = (double)cv::getTickCount();
        vector<vector<cv::DMatch> > knnMatches;
        matcher->knnMatch(descSource, descRef, knnMatches, k); // Finds the kNN match for each descriptor in desc1
        // TODO : filter matches using descriptor distance ratio test
        const float ratio_thresh = 0.8f;

        for (size_t i = 0; i < knnMatches.size(); i++)
        {
            if (knnMatches[i][0].distance < ratio_thresh * knnMatches[i][1].distance)
            {
                matches.push_back(knnMatches[i][0]);
            }
        }

        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << " (kNN) with n=" << matches.size() << " matches in " << 1000 * t / 1.0 << " ms" << endl;
    }

    // visualize results
    cv::Mat matchImg = imgRef.clone();
    cv::drawMatches(imgSource, kPtsSource, imgRef, kPtsRef, matches,
                    matchImg, cv::Scalar::all(-1), cv::Scalar::all(-1), vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    string windowName = "Matching keypoints between two camera images (best 50)";
    cv::namedWindow(windowName, 7);
    cv::imshow(windowName, matchImg);
    cv::waitKey(0);
}

bool checkArg(char* argv)
{
    if (atoi(argv) > 2)
    {
        cout << "Argument Error: " << argv << endl;
        return false;
    }
    return true;
}

bool getArguments(vector<char> &args, int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (atoi(argv[i]) > 2)
        {
            cout << "Argument Error: " << argv[i] << endl;
            return false;
        }
 
        args[i-1] = atoi(argv[i]);
    }
    return true;
}

int main(int argc, char** argv)
{
    cv::Mat imgSource = cv::imread("../images/img1gray.png");
    cv::Mat imgRef = cv::imread("../images/img2gray.png");

    string header = "../dat/C35A5_";
    string typeset[4] = {"KptsSource_", "KptsRef_", "DescSource_", "DescRef_"};
    string dataset[3] = {"BRISK_small", "BRISK_large", "SIFT"};
    string ext = ".dat";

    vector<char> args = {0, 0, 0};
    if (!getArguments(args, argc, argv))
    {
        return 0;
    }

    vector<cv::KeyPoint> kptsSource, kptsRef; 
    readKeypoints((header + typeset[0] + dataset[args[0]] + ext).c_str(), kptsSource);
    readKeypoints((header + typeset[1] + dataset[args[0]] + ext).c_str(), kptsRef);

    cv::Mat descSource, descRef;
    readDescriptors((header + typeset[2] + dataset[args[0]] + ext).c_str(), descSource);
    readDescriptors((header + typeset[3] + dataset[args[0]] + ext).c_str(), descRef);

    vector<cv::DMatch> matches;
    string matcherType[2] = {"MAT_BF", "MAT_FLANN"};
    string descriptorType = "DES_BINARY"; 
    string selectorType[2] = {"SEL_NN", "SEL_KNN"};
    matchDescriptors(imgSource, imgRef, kptsSource, kptsRef, descSource, descRef, matches, descriptorType, matcherType[args[1]], selectorType[args[2]]);
}