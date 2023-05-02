// Include files for required libraries
#include <stdio.h>
#include <iostream>
#include <list>
#include "opencv_aee.hpp"
#include "main.hpp"     // You can use this file for declaring defined values and functions
#include "pi2c.h"  //I2C header

using namespace std;
using namespace cv;
//global variable of arduino I2C ID
#define ArdID 0x08


//int master = wiringPiI2CSetup(0x53); //setup master with address 0x22
float PIDError;
const int setpoint = 160;

//Symbol Recognition Function
/*int symbolRecognition(Mat imgHSV)
{
    Mat pink;

    inRange(imgHSV, Scalar(150, 20, 50), Scalar(170, 255, 255), pink);      //magenta hues are singled out and a black/white image is returned

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

    Mat transformed = transformPerspective(approxedContours[iNum], pink, 350, 350);     //transform perspective

    Mat umbrella = imread("Umbrella.png");
    cvtColor(umbrella, umbrella, COLOR_BGR2HSV);
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

    int matchSorted[4] = {umbrellaMatch, circleMatch, triangleMatch, starMatch};
    sort(matchSorted, matchSorted+4);
    printf("%d\t%d\t%d\t%d\n", matchSorted[0], matchSorted[1], matchSorted[2], matchSorted[3]);
    if(matchSorted[3] >= 75)
    {
        if(matchSorted[3] == umbrellaMatch)
        {
            return 2;
        }
        else if(matchSorted[3] == circleMatch)
        {
            return 1;
        }
        else if(matchSorted[3] == triangleMatch)
        {
            return 4;
        }
        else
        {
            return 3;
        }
    }
    else
    {
        return 0;
    }
}*/


void setup(void)
{
    setupCamera(320, 240);  // Enable the camera for OpenCV

}





int main( int argc, char** argv )
{
    setup();    // Call a setup function to prepare IO and devices

    Pi2c arduino(8);        //creates new object called aduino at address 0x08

    cv::namedWindow("Photo");   // Create a GUI window called photo

    int symbolPrev = 5;
    int symbol = 5;

    while(1)    // Main loop to perform image processing
    {
        Mat frame, frameFlipped;

        while(frame.empty())
            frame = captureFrame(); // Capture a frame from the camera and store in a new matrix variable
        // get the center coordinates of the image to create the 2D rotation matrix

        Point2f center((frame.cols / 2.0), (frame.rows / 2.0));
        // using getRotationMatrix2D() to get the rotation matrix
        Mat rotation_matrix = getRotationMatrix2D(center, 180, 1.0);

        // rotate the image using warpAffin
        warpAffine(frame, frameFlipped, rotation_matrix, frame.size());
        Mat frameHSV;
        cvtColor(frameFlipped, frameHSV, COLOR_BGR2HSV);

        Mat pink;
        inRange(frameHSV, Scalar(150, 20, 50), Scalar(170, 255, 255), pink);
        int pixels = countNonZero(pink);
        if(pixels > 10)
        {
            printf("\n pink detected");
            //symbol = symbolRecognition(frameHSV);
            if(symbol == 0)
            {
                symbol = symbolPrev;

            }
        }
        symbolPrev = symbol;
        printf("\n %d\t", symbol);


        if (symbol == 1)    //for red
        {
            Mat red1, red2;
            inRange(frameHSV, Scalar(0, 50, 50), Scalar(15, 255, 255), red1);
            inRange(frameHSV, Scalar(170, 50, 50), Scalar(255, 255, 255), red2);
            frameHSV = (red1+red2);
        }
        else if(symbol == 2)    //for yellow
        {
            inRange(frameHSV, Scalar(20, 50, 50), Scalar(35, 255, 255), frameHSV);
        }
        else if(symbol == 3)    //for green
        {
            inRange(frameHSV, Scalar(75, 50, 50), Scalar(90, 255, 255), frameHSV);
        }
        else if(symbol == 4)    //for blue
        {
            inRange(frameHSV, Scalar(90, 50, 50), Scalar(130, 255, 255), frameHSV);
        }
        else if(symbol == 5)    //for black
        {
            inRange(frameHSV, Scalar(0, 0, 0), Scalar(255, 255, 50), frameHSV);
        }
        //cvtColor(frameHSV, frameHSV, COLOR_GRAY2BGR);
        Mat kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
        cv::morphologyEx(frameHSV, frameHSV, MORPH_OPEN, kernel);

        int numerator = 0, denominator = 0, servoAngle;
        double average = 0, sumError, prevError, u;
        char stringAngle[3], transmission[20], encoderDisStr[8], buzzword[8];

        strcpy(buzzword, "stp");    //sets car to go forward
        uchar* p = frameHSV.ptr<uchar>(120);
        for(int x = 0; x < 320; x++)
        {
            p[x]; // This is our B&W pixel data we can read it or write it
            uchar pixel = p[x]; // read the data into pixel
            if (p[x] == 0){
                numerator += p[x] * x;   //if 0, do 0*position
                denominator += p[x];
            }
            else {
                numerator += 1 * x;     //else, do 1 * position, so the random value for white does not skew the error
                denominator += 1;
            }

        }

        denominator += 1;

        average = numerator/denominator;

        prevError = PIDError;
        PIDError = setpoint - average;
        sumError += PIDError;

        if (average == 0){       //if no line detected, have no P value
            PIDError = 0;
            strcpy(buzzword, "bak");
        }


        u = (0.5*(PIDError)) + (0.0000*(sumError)) + (0.00*(PIDError - prevError));


        servoAngle = 59 - u ; //servo angle is centre angle - PID value


        if (servoAngle > 999)   //keeps servo angle positive and maximum 3 digits
            servoAngle = 999;
        else if (servoAngle < 0)
            servoAngle = 0;





        cv::imshow("Photo", frameHSV); //Display the image in the window


        gcvt(servoAngle, 10, stringAngle);
        if (servoAngle < 100)
            strcat(stringAngle, "-");   //adds a - at the end of the angle string so that if the angle is only 2 digits, it still takes 3 characters
        else if (servoAngle < 10)
            strcat(stringAngle, "--");  //adds a -- at the end of the angle string so that if the angle is only 1 digit, it still takes 3 characters
        strcpy(transmission, stringAngle);
        strcat(transmission, buzzword);  //concatinates the servo angle and buzzword


        arduino.i2cWrite(transmission,6);



        int key = cv::waitKey(1);   // Wait 1ms for a keypress (required to update windows)

        key = (key==255) ? -1 : key;    // Check if the ESC key has been pressed
        if (key == 27)
            break;
    }

    closeCV();  // Disable the camera and close any windows

    return 0;
}





