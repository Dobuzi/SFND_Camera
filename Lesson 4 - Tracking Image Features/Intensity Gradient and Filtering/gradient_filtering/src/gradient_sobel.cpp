#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

void gradientSobel()
{
    // TODO: Based on the image gradients in both x and y, compute an image 
    // which contains the gradient magnitude according to the equation at the 
    // beginning of this section for every pixel position. Also, apply different 
    // levels of Gaussian blurring before applying the Sobel operator and compare the results.
    cv::Mat img;
    img = cv::imread("../images/img1.png");

    cv::Mat imgGray;
    cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

    float sobels[2][9] = {{-1, 0, 1, // sobel x
                        -2, 0, 2,
                        -1, 0, 1},
                        {-1, -2, -1, // sobel y
                        0, 0, 0,
                        1, 2, 1}};
    string dir[2] = {"x", "y"};
    string windowNames[2] = {"Sobel operator (", "-direction)"};
    string windowName;
    cv::Mat result;

    for (int i = 0; i < 2; i++)
    {
        cv::Mat kernel = cv::Mat(3, 3, CV_32F, sobels[i]);
        cv::filter2D(imgGray, result, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

        windowName = windowNames[0] + dir[i] + windowNames[1];

        cv::namedWindow(windowName, 1);
        cv::imshow(windowName, result);
        cv::waitKey(0);

        cv::destroyWindow(windowName);
    }
}

int main()
{
    gradientSobel();
}