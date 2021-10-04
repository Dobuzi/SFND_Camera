#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <cmath>

using namespace std;

void magnitudeSobel()
{
    // load image from file
    cv::Mat img;
    img = cv::imread("../images/img1gray.png");

    // convert image to grayscale
    cv::Mat imgGray;
    cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

    // apply smoothing using the GaussianBlur() function from the OpenCV
    // ToDo : Add your code here
    cv::Mat imgBlur;
    cv::GaussianBlur(imgGray, imgBlur, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);

    // create filter kernels using the cv::Mat datatype both for x and y
    // ToDo : Add your code here
    float sobel_x[9] = {-1, 0, 1,
                        -2, 0, 2,
                        -1, 0, 1};
    float sobel_y[9] = {-1, -2, -1,
                        0, 0, 0,
                        1, 2, 1};
    cv::Mat kernel_x = cv::Mat(3, 3, CV_32F, sobel_x);
    cv::Mat kernel_y = cv::Mat(3, 3, CV_32F, sobel_y);

    // apply filter using the OpenCv function filter2D()
    // ToDo : Add your code here
    cv::Mat imgFilterX, imgFilterY;
    cv::filter2D (imgBlur, imgFilterX, -1, kernel_x, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
    cv::filter2D (imgBlur, imgFilterY, -1, kernel_y, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

    // compute magnitude image based on the equation presented in the lesson 
    // ToDo : Add your code here
    
    int nrows = imgBlur.rows;
    int ncols = imgBlur.cols;

    cv::Mat magnitude;
    magnitude.create(nrows, ncols, CV_8UC1);

    bool selfmade = true;

    if (selfmade)
    {
        float gradient, gradient_x, gradient_y, theta, norm_grad;
        int delta = 1;
    
        float gradients[nrows-delta][ncols-delta];
    
        float max_grad = 0;
        float min_grad = 255;

        for (int i = 0; i < nrows-delta; i++)
        {
            for (int j = 0; j < ncols-delta; j++)
            {
                gradient_x = (imgBlur.at<unsigned char>(i, j+delta) - imgBlur.at<unsigned char>(i, j)) / delta;
                gradient_y = (imgBlur.at<unsigned char>(i+delta, j) - imgBlur.at<unsigned char>(i, j)) / delta;
                gradient = sqrt(gradient_x * gradient_x + gradient_y * gradient_y);
                // theta = atan(gradient_x / gradient_y);
                gradients[i][j] = gradient;
                max_grad = max_grad < gradient ? gradient : max_grad;
                min_grad = min_grad > gradient ? gradient : min_grad;
            }
        }

        for (int i = 0; i < nrows-delta; i++)
        {
            for (int j = 0; j < ncols-delta; j++)
            {
                norm_grad = (gradients[i][j] - min_grad)/(max_grad - min_grad);
                magnitude.at<unsigned char>(i, j) = int(abs(norm_grad * 255));
            }
        }
    }
    else
    {
        for (int i = 0; i < nrows; i++)
        {
            for (int j = 0; j < ncols; j++)
            {
                magnitude.at<unsigned char>(i, j) = sqrt(pow(imgFilterX.at<unsigned char>(i, j), 2) + pow(imgFilterY.at<unsigned char>(i, j), 2));
            }
        }
    }

    // show result
    string windowName = "Gaussian Blurring";
    cv::namedWindow(windowName, 1); // create window
    cv::imshow(windowName, magnitude);
    cv::waitKey(0); // wait for keyboard input before continuing
}

int main()
{
    magnitudeSobel();
}