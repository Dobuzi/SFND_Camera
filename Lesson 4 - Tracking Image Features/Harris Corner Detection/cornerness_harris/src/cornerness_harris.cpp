#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

using namespace std;

void cornernessHarris()
{
    // load image from file
    cv::Mat img;
    img = cv::imread("../images/img1.png");
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY); // convert to grayscale

    // Detector parameters
    int blockSize = 2;     // for every pixel, a blockSize × blockSize neighborhood is considered
    int apertureSize = 3;  // aperture parameter for Sobel operator (must be odd)
    int minResponse = 100; // minimum value for a corner in the 8bit scaled response matrix
    double k = 0.04;       // Harris parameter (see equation for details)

    // Detect Harris corners and normalize output
    cv::Mat dst, dst_norm, dst_norm_scaled;
    dst = cv::Mat::zeros(img.size(), CV_32FC1);
    cv::cornerHarris(img, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT);
    cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
    cv::convertScaleAbs(dst_norm, dst_norm_scaled);

    // visualize results
    // string windowName = "Harris Corner Detector Response Matrix";
    // cv::namedWindow(windowName, 4);
    // cv::imshow(windowName, dst_norm_scaled);
    // cv::waitKey(0);

    // TODO: Your task is to locate local maxima in the Harris response matrix 
    // and perform a non-maximum suppression (NMS) in a local neighborhood around 
    // each maximum. The resulting coordinates shall be stored in a list of keypoints 
    // of the type `vector<cv::KeyPoint>`.
    vector<cv::KeyPoint> kpts;
    unsigned char response;
    cv::Scalar color;

    for (int i = 0; i < dst_norm_scaled.rows; i++)
    {
        for (int j = 0; j < dst_norm_scaled.cols; j++)
        {
            response = dst_norm_scaled.at<unsigned char>(i, j);
            if (response > minResponse)
            {
                cv::KeyPoint pt;
                pt.pt = cv::Point2f(j, i);
                pt.size = apertureSize * 2;
                pt.response = response;

                bool isOverlapped = false;
                for (auto it = kpts.begin(); it != kpts.end(); ++it)
                {
                    if (cv::KeyPoint::overlap (pt, *it))
                    {
                        isOverlapped = true;
                        if (pt.response > (*it).response)
                        {
                            *it = pt;
                            break;
                        }
                    }
                }

                if (!isOverlapped)
                {
                    kpts.push_back(pt);
                }
            }
        }
    }

    // visualize results
    cv::Mat dst_kpts;
    cv::drawKeypoints (dst_norm_scaled, kpts, dst_kpts);

    string windowName = "Harris Corner Detector Result";
    cv::namedWindow(windowName, 4);
    cv::imshow(windowName, dst_kpts);
    cv::waitKey(0);
}

int main()
{
    cornernessHarris();
}