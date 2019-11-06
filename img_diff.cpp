// LAUNCH IT
// g++ img_diff.cpp -o main `pkg-config --cflags --libs opencv` -g && ./main
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;


int filter(Rect box, int width, int height) {
    if (
        box.width < width * 0.7 && 
        box.width > width * 0.1 && 
        box.height < height * 0.7 && 
        box.height > height * 0.1
    ) 
    {
        return 1;
    };
    return 0;
}

int main (void) {
    
    using std::vector;

    Mat prev_frame;
    VideoCapture cap;
    int f_i = 0;
    int width;
    int height;

    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(1))
        return 0;
    for(;;)
    {
        Mat frame;
        cap >> frame;
        if( frame.empty() ) break; // end of video stream
        cv::cvtColor(frame, frame, CV_BGR2GRAY);
        
        // initialize prev_frame as current frame, because in the begining we don't have a previous image
        if(f_i == 0) {
            prev_frame = frame;
            width = frame.size().width;
            height = frame.size().height;
        }

        // inorder to perform the absdiff, the images need to be of equal size
        // NOTE: I believe this is the reason why you have a big blue box on the edges
        // as well of the output image. This can be fixed by using two equal images in the first place    
        Mat diff;
        absdiff(prev_frame, frame, diff);
        
        imshow("DIFFERENCE", diff);
        // std::cout << "diff" << diff << std::endl;

        // after getting the difference, we binarize it
        Mat thresh;
        threshold(diff, thresh, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        // extract the contours in the threshold image
        findContours(thresh, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

        // this matrix will be used for drawing purposes
        Mat out;
        cvtColor(frame, out, COLOR_GRAY2BGR);

        // For each detected contour, calculate its bounding rectangle and draw it
        // You can also filter out some noise should you wish by checking if the contour is big
        // or small enough along with other contour properties.
        // Ref: https://docs.opencv.org/trunk/dd/d49/tutorial_py_contour_features.html
        // Ref: https://docs.opencv.org/trunk/d1/d32/tutorial_py_contour_properties.html
        for(vector<Point> cont: contours)
        {
            Rect box = boundingRect(cont);
            if (filter(box, width, height)) {
                rectangle(out, box, Scalar(255, 0, 0));
            }
        }

        // imshow("FIRST", first);
        // imshow("SECOND", second);
        // imshow("ABS-DIFF", diff);
        // imshow("THRESH", thresh);
        imshow("OUTPUT", out);

        // imshow("this is you, smile! :)", frame);
        prev_frame = frame;
        f_i += 1;
        if( waitKey(10) == 27 || waitKey(10) == 27 ) { // stop capturing by pressing ESC 
            destroyAllWindows();
            break;
        }
    }

}
