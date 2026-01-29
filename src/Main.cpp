// Include opencv and other header files
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

// Using namespaces
using namespace std;
using namespace cv;

// A function that measures the diameter of the cable at three points — 1/4, 1/2, and 3/4 of the cable height from the top of the image.
// Parameters are Mat object and an integer value denoting the row of the matrix. 
// It returns a tuple of two integers, which are starting and ending points of the diameter.

tuple<int, int> measurediameter(Mat tgt, int row)
{
    int measurediameter1 = 0, flag = 0, point1, point2, j;

    // This for loop gets the starting point of the diameter (left to right)
    for (j = 0; j < tgt.cols; j++)
    {
        // The if condition checks if a pixel is white.The cable is considered to start with white (bright) pixel.
        if ((int)tgt.at<uchar>(row, j) == 255)
        {
            // Breaks the for loop when the first point is found.
            point1 = j;
            break;
        }
    }
    // This for loop gets the ending point of the diameter by starting to check from the 
    // last column and keeps on moving towards the first column (right to left).
    for (j = tgt.cols - 1; j >= 0; j--)
    {
        // The if condition checks if a pixel is white.
        if ((int)tgt.at<uchar>(row, j) == 255)
        {
            // Breaks the for loop when the last point is found.
            point2 = j;
            break;
        }
    }

    // Prints the diameter of the cable at a particular row.
    cout << "Diameter: " << point2 - point1 + 1 << "\n";
    // Returns the first point and last in the form of a tuple.
    return make_tuple(point1, point2);
}

// A function to draw lines indicating the diameter of the cable and for writing on the image.
// The function takes in the image to draw and write on (org_img), the x axis values of the 
// starting and ending points of the diameter (p1, p2) and y value for the two points (row).

void draw_lines(Mat org_img, int p1, int p2, int row)
{
    int thickness = 2;

    // Measure the diameter by calculating x-axis of last point - x-axis of the first point + 1.
    int dia = p2 - p1 + 1;

    // Create two points by specifying the x axis and y axis of those two points.
    Point p11left(p1, row), p12left(p1 - 20, row);
    Point p11right(p2, row), p12right(p2 + 20, row);

    // Draw two lines, one on the left side and another on the right side of the cable.
    // The parameters to the line function are image to draw on, starting and ending points of the
    // line, color of the line, thickness, and how the line will be drawn.
    line(org_img, p11left, p12left, Scalar(0, 0, 255), thickness, LINE_8);
    line(org_img, p11right, p12right, Scalar(0, 0, 255), thickness, LINE_8);

    // Write the text specifying the diameter of the cable at a specific point.
    // Passing the image to write on, text, starting point to write, text font, font scale,
    // color, and thickness of the line to the putText function.
    putText(org_img, "Diameter " + to_string(dia), Point(p2 + 50, row + 6),
        FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 0, 255), 2);
}

// A function to detect and classify defects. The function takes in an image containing edges (edges_img)
// and the image (img) to draw the circles around the defects.

void defects(Mat edges_img, Mat original_img)
{
    // Create a vector to hold vector of Point objects
    vector<vector<Point> > contours;

    // Find the contours from the image containing the edges. 
    // Passing source, destination, contour retrieval mode, and contour approximation method.   
    findContours(edges_img, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // Create a vector to hold vector of Point object with the same size as the size of contours vector.
    vector<vector<Point> > contours_poly(contours.size());
    vector<Point2f>centers(contours.size());
    vector<float>radius(contours.size());
    Mat img_with_circles = original_img.clone();
    Mat img_with_circles_and_text = original_img.clone();

    // Using a for loop to go through every contour and find minimum enclosing circle for every ploygon
    // and save it to radius and center vectors. The vector radius contains radius of each circle and centers
    // contain centers of the circles.
    for (size_t i = 0; i < contours.size(); i++)
    {
        approxPolyDP(contours[i], contours_poly[i], 3, true);
        minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
    }

    // get the maximum value of the radius vector
    float max = *max_element(radius.begin(), radius.end());
    int count_pn = 0;

    // Going through each radius and removing maximum and another radius closest to maximum if the
    // difference between them is less 1. This is because the cable is considered as a contour and two circles 
    // are drawn around the cable. Removing some other radiuses if they are less than a certain threshold. 
    for (size_t i = 0; i < radius.size(); i++)
    {

        if (radius[i] == max)
        {
            radius[i] = 0;
        }
        if (max - radius[i] < 1.0)
        {
            radius[i] = 0;
        }
        if (radius[i] < 5.0)
        {
            radius[i] = 0;
        }
    }

    // Get the maximum radius and maximum radius index from the radius vector.
    max = *max_element(radius.begin(), radius.end());
    int mxI = max_element(radius.begin(), radius.end()) - radius.begin();

    // Now we want to remove the circles which intersect with the largest circle, or are inside the largest
    // one, touches the largest circle, or is close to the largest one by a threshold. This is done by first 
    // calculating the distance.
    for (size_t i = 0; i < radius.size(); i++)
    {
        // Calculate the distance between two circles by using the formula square root of (x1-x2)^2 + (y1-y2)^2
        // where the center of one circle is (x1, y1) and center of the second circle is (x2, y2). 
        double d = sqrt((centers[i].x - centers[mxI].x) *
            (centers[i].x - centers[mxI].x) + (centers[i].y - centers[mxI].y) *
            (centers[i].y - centers[mxI].y));
        if (d < radius[mxI] - radius[i]) {
            //Smaller circle is inside largest circle. So we will remove it.
            radius[i] = 0;
        }
        else if (d == radius[mxI] - radius[i]) {
            // We will keep the largest circle.
        }

        else if (d < radius[i] + radius[mxI]) {
            // If a circle intersects with largest circle we will remove it.
            radius[i] = 0;
        }
        else if (d == radius[i] + radius[mxI]) {
            // If a circle touches the largest circle we will remove it.
            radius[i] = 0;
        }
        else if (d < 100) {
            radius[i] = 0;
            // If a circle does not intersect or touch or is not within the larger circle but is closer 
            // to the largest circle by a threshold we will remove the circle.
        }
        // Create image copies for saving
    }
    
    // Going through all the contours and drawing circles around them in the original image.
    for (size_t i = 0; i < contours.size(); i++)
    {
        // If the radius is not 0 we are going to draw the circle and write the name of the defect.
        if ((int)radius[i] != 0)
        {
            // Draw the circle by specifying the image to draw on, center of a circle, radius of the circle
            // color of the circle, and the thickness of the circle outline.
            circle(img_with_circles, centers[i], (int)radius[i] + 10, Scalar(0, 255, 0), 2);
            circle(img_with_circles_and_text, centers[i], (int)radius[i] + 10, Scalar(0, 255, 0), 2);
            // If the radius is less than 10 we are classifying the defect as pin hole.
            if ((int)radius[i] <= 10)
                putText(img_with_circles_and_text, "Defect: Pin Hole", Point(centers[i].x + 70, centers[i].y + 30),
                    FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 100, 255), 2);

            // If the radius is greater than 10 and less than 40 we are classifying the defect as cut.
            if ((int)radius[i] > 10 && (int)radius[i] <= 40)
                putText(img_with_circles_and_text, "Defect: Cut", Point(centers[i].x + 70, centers[i].y + 30),
                    FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 100, 255), 2);

            // If the radius is greated than 40 we will classify the defect as scratch.
            if ((int)radius[i] > 40)
                putText(img_with_circles_and_text, "Defect: Scratch", Point(centers[i].x + 70, centers[i].y + 30),
                    FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 100, 255), 2);
        }
    }
    // save the output images (answers to Exercise 2 & 3)
    imwrite("C:/Users/niloofae/Downloads/CableDefectDetection/Exercise2_output/defectoutput1_Pin-Hole_and_cut.jpg", img_with_circles);
    imwrite("C:/Users/niloofae/Downloads/CableDefectDetection/Exercise3_output/classifyoutput1_Pin-Hole_and_cut.jpg", img_with_circles_and_text);
}

int main()
{
    // Specify the location of the folder with the input cable images and read the input image.
    Mat img = imread("C:/Users/niloofae/Downloads/CableDefectDetection/Pin-Hole_and_cut.bmp");
    int i = 0, j = 0;

    Mat img_gray;
    // Convert the image to graycsale and store the grayscale image in img_gray.
    cvtColor(img, img_gray, COLOR_BGR2GRAY);

    // Convert the black background of the grayscale img_gray to white. This helps with detecting edges of the cables.
    // Make all the pixels outside of the center 240 pixels white. This helps to remove noises present in the images.
    for (i = 0; i < img_gray.rows; i++)
        for (j = 0; j < img_gray.cols; j++)
            if ((int)img_gray.at<uchar>(i, j) < 40 || j < (img_gray.cols / 2) - 120 || j >(img_gray.cols / 2) + 120)
                img_gray.at<uchar>(i, j) = 255;

    // Show the grayscale image in a window (Just for debugging)
    imshow("Gray Image", img_gray);

    Mat edges_img;
    int threshold = 150;

    // Applying Canny edge detection for detecting edges from the grayscale image.
    // Sending source, destination, first threshold, second threshold, aperture size for the Sobel operator, and
    // a flag indicating whether L2 norm or L1 norm will be used to calculate the image gradient.
    Canny(img_gray, edges_img, threshold, threshold * 2, 3, false);

    // Show the detected edges.
    imshow("Edges", edges_img);

    Mat img_defect_circles = img.clone();

    int p1, p2;

    // Diameter at 1/4 of image height (top part diameter)
    std::tie(p1, p2) = measurediameter(edges_img, img.rows / 4);
    draw_lines(img, p1, p2, img.rows / 4);

    // Diameter at 1/2 of image height (middle part diameter)
    std::tie(p1, p2) = measurediameter(edges_img, img.rows / 2);
    draw_lines(img, p1, p2, img.rows / 2);

    // Diameter at 3/4 of image height (bottom part diameter)
    std::tie(p1, p2) = measurediameter(edges_img, 3 * img.rows / 4);
    draw_lines(img, p1, p2, 3 * img.rows / 4);

    // Save the result of the diameter measurement (Answer to Exercise1)
    cv::imwrite("C:/Users/niloofae/Downloads/CableDefectDetection/Exercise1_output/diameteroutput_Pin-Hole_and_cut.jpg", img);

    // Call the defects function to detect and classify the defects and save the images inside the function (Answer to Exercise 2 & 3)
    defects(edges_img, img_defect_circles);
    waitKey(0);
    destroyAllWindows();
    return 0;
}