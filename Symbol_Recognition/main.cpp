//Include files for required libraries
#include <stdio.h>
#include "opencv_aee.hpp"
#include <list>
#include <iostream>
#include "main.hpp"

using namespace std;
using namespace cv;

int symbolRecognition(Mat image)    //function symbolRecognition() is declared
{
    Mat image = imread("testTriangle.jpg");       //reads from a file within the same directory
    Mat imgHSV;
    cvtColor(image, imgHSV, COLOR_BGR2HSV);     //image is converted to HSV format
    Mat pink;
    inRange(imgHSV, Scalar(150, 20, 50), Scalar(170, 255, 255), pink);      //magenta hues are singled out and a black/white image is returned
    int pixels = 0;
    pixels = countNonZero(pink);
    printf("Non-zero:\t%d\n", pink);
    Mat kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    cv::morphologyEx(pink, pink, MORPH_OPEN, kernel);       //morphs are used to make the image less complex

    std::vector< std::vector<cv::Point> > contours;     //find contours
    std::vector<Vec4i> hierarchy;       //vector "hierarchy" is declared
    cv::findContours(pink, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    std::vector< std::vector<cv::Point> > approxedContours(contours.size());        //simplify contours
    for(uint i = 0; i < contours.size(); i++)
    {
        cv::approxPolyDP(contours[i],approxedContours[i], 10, true);
    }

    int area, iNum, areaPrev = 0;
    for (int i = 0; i < approxedContours.size(); i++)       //find largest contour
    {
        area = cv::contourArea(approxedContours[i]);
        if (area > areaPrev)
        {
            iNum = i;
        }
        areaPrev = area;
    }

    drawContours(image, approxedContours, iNum, Scalar(255, 0, 0), 2, LINE_8, noArray(), 0, Point());

    Mat transformed = transformPerspective(approxedContours[iNum], pink, 350, 350);     //transform perspective
    Mat umbrella = imread("Umbrella.png");
    cvtColor(umbrella, umbrella, COLOR_BGR2HSV);        //each known symbol is read and converted to GRAY
    inRange(umbrella, Scalar(150, 20, 50), Scalar(170, 255, 255), umbrella);
    Mat circle = imread("Circle.png");
    cvtColor(circle, circle, COLOR_BGR2HSV);
    inRange(circle, Scalar(150, 20, 50), Scalar(170, 255, 255), circle);
    Mat triangle = imread("Triangle.png");
    cvtColor(triangle, triangle, COLOR_BGR2HSV);
    inRange(triangle, Scalar(150, 20, 50), Scalar(170, 255, 255), triangle);
    Mat star = imread("Star.png");
    cvtColor(star, star, COLOR_BGR2HSV);
    inRange(star, Scalar(150, 20, 50), Scalar(170, 255, 255), star);
    int umbrellaMatch = compareImages(transformed, umbrella);        //compare to known symbols
    int circleMatch = compareImages(transformed, circle);
    int triangleMatch = compareImages(transformed, triangle);
    int starMatch = compareImages(transformed, star);
    int matchSorted[4] = {umbrellaMatch, circleMatch, triangleMatch, starMatch};	//create an array for all outputs
    sort(matchSorted, matchSorted+4);	//sort the array from start to finish
    printf("%d\t%d\t%d\t%d\n", matchSorted[0], matchSorted[1], matchSorted[2], matchSorted[3]);	//output percentages in order
    if(matchSorted[3] >= 75)    //percentage match to each symbol is output
    {
        if(matchSorted[3] == umbrellaMatch)
        {
            printf("Closest match is umbrella");
            return 2;
        }
        else if(matchSorted[3] == circleMatch)
        {
            printf("Closest match is circle");      //closest match is determined and output
            return 1;
        }
        else if(matchSorted[3] == triangleMatch)
        {
            printf("Closest match is triangle");
            return 4;
        }
        else
        {
            printf("Closest match is star");
            return 3;
        }
    }
    else
    {
        printf("No close match");
        return 0;
    }
}

int main( int argc, char** argv )
{
    Mat image = imread("testCircle.jpg");
    int symbol = symbolRecognition(image);      //function called in main
    imshow("Image", image); //original image is output
    waitKey();
}

