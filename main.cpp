#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>

bool checkImageEmpty(cv::Mat image) {
    if (image.empty()) {
        std::cerr << "Error: Could not open or find the image!" << std::endl;
        return true;
    }
    else {
        return false;
    }
};


cv::Rect detectIndicator(cv::Mat image, int paddingSize=10) {
    // variables
    cv::Mat copyImage;
    cv::Mat grayImage;
    cv::Mat paddedImage;
    cv::Mat binaryImage;
    cv::Mat kernel;
    std::vector<std::vector<cv::Point>> contours;
    cv::Rect boudingRect = cv::Rect(0,0,0,0);
    cv::Rect emptyRect = cv::Rect(0,0,0,0);

    // add padding
    cv::copyMakeBorder(image,paddedImage,paddingSize,paddingSize,paddingSize,paddingSize,cv::BORDER_CONSTANT,cv::Scalar(255,255,255));
    if (checkImageEmpty(paddedImage)) {
        return emptyRect;
    }

    // change the image to grayscale
    cv::cvtColor(paddedImage,grayImage,cv::COLOR_BGR2GRAY);
    if (checkImageEmpty(grayImage)) {
        return emptyRect;
    }

    // thresholding
    cv::threshold(grayImage,binaryImage,250,255,cv::THRESH_BINARY);
    if (checkImageEmpty(binaryImage)) {
        return emptyRect;
    }

    // morphological operations
    kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(3,3));
    cv::morphologyEx(binaryImage,binaryImage,cv::MORPH_CLOSE,kernel,cv::Point(-1,-1),5);
    cv::dilate(binaryImage,binaryImage,kernel,cv::Point(-1,-1),5);

    // detect indicator
    cv::findContours(binaryImage,contours,cv::RETR_CCOMP,cv::CHAIN_APPROX_NONE);
    if (contours.empty()) {
        return emptyRect;
    }
    boudingRect = cv::boundingRect(contours[0]);
    boudingRect.x -= paddingSize; //remove the padding
    boudingRect.y -= paddingSize; //remove the padding

    //show the indicator
    image.copyTo(copyImage);
    cv::rectangle(copyImage,boudingRect,cv::Scalar(255,0,0));
    // cv::imshow("Image with indicator",copyImage);

    return boudingRect;
};

void plotHistogram(cv::Mat image, int type=0, bool save=false) {
    // variables
    cv::Mat copyImage;
    cv::Mat grayImage;
    cv::Mat hist;
    int histSize = 256;
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange[] = { range };
    bool uniform = true, accumulate = false;
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );
    int paddingSize = 10;

    // convert image to grayscale
    cv::cvtColor(image,grayImage,cv::COLOR_BGR2GRAY);
    if (checkImageEmpty(grayImage)) {
        return;
    }

    // form the histogram
    cv::calcHist(&grayImage, 1, 0, cv::Mat(), hist, 1, &histSize, histRange, uniform, accumulate);

    // visualize the histogram 
    cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar(0,0,0) );
    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

    //line type
    switch (type) {
        case 1: {
            for( int i = 1; i < histSize; i++ )
            {
                cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ),
                    cv::Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
                    cv::Scalar( 255, 255, 255), 1);
            }
            break;
        }
            
        default: {
            // bar type
            for( int i = 0; i < histSize; i++ )
            {
                for (int j = 0; j < bin_w; j++) {
                    cv::line(histImage,cv::Point((bin_w*i)+j,hist_h), cv::Point((bin_w*i)+j,hist_h-cvRound(hist.at<float>(i))),
                    cv::Scalar(255,255,255),1);
                }
            }
            break;
        }
        
    }

    cv::copyMakeBorder(histImage,histImage,paddingSize,paddingSize,paddingSize,paddingSize,cv::BORDER_CONSTANT,cv::Scalar(0,0,0));
    if (save) {
        cv::imwrite("./histogram.png",histImage);
    }
    cv::namedWindow("Histogram");
    cv::moveWindow("Histogram", 0, 0);
    cv::imshow("Histogram", histImage);

}

int main(int argc, char** argv){
    //check if opencv is installed
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    // Suppress OpenCV log messages
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    // variables
    cv::Rect indicatorRect = cv::Rect(0,0,0,0);
    cv::Point imageCenter = cv::Point(0,0);
    cv::Point indicatorCenter = cv::Point(0,0);
    cv::Mat image;
    std::string imagePath = "image.png";
    int histogramType = 0;
    bool saveOutputs = false;

    // Parse the arguments
    // std::cout << "argc :" << argc << std::endl;
    for (int i=0; i<argc; i++) {
        std::string arg = argv[i];
        if (arg.find("--input=") == 0) {
            imagePath = arg.substr(8); // Extract value after "--input="
        }
        else if (arg.find("--histogram_type=") == 0) {
            std::string value = arg.substr(17); // Extract value after "--histogram_type="
            try {
                int type = stoi(value);
                if (type >=0 && type<=1) {
                    histogramType = type;
                }
                else {
                    throw -1;
                }
            }
            catch(...) {
                std::cerr << "Invalid histogram type:" << value << ", will use default value";
            }
        }
        else if (arg == "--save_outputs") {
            saveOutputs = true;
        }

        // std::cout << i << ":argc :" << arg << std::endl;
    }


    image = cv::imread(imagePath, cv::IMREAD_COLOR); // Read the image in color
    // Check if the image was loaded successfully
    if (checkImageEmpty(image)) {
        return -1;
    }

    imageCenter.x = image.cols/2;
    imageCenter.y = image.rows/2;

    // Detect the position of indicator
    indicatorRect = detectIndicator(image);
    indicatorCenter.x = indicatorRect.x + (indicatorRect.width/2);
    indicatorCenter.y = indicatorRect.y + (indicatorRect.height/2);
    
    // std::cout << imageCenter << std::endl;
    // std::cout << indicatorCenter << std::endl;

    // Auto rotate
    if (indicatorCenter.x > imageCenter.x && indicatorCenter.y < imageCenter.y) {
        // 2nd quardrant
        cv::rotate(image,image,cv::ROTATE_90_COUNTERCLOCKWISE);
    }
    else if (indicatorCenter.x < imageCenter.x && indicatorCenter.y > imageCenter.y) {
        // 3rd quardrant
        cv::rotate(image,image,cv::ROTATE_90_CLOCKWISE);
    }
    else if (indicatorCenter.x > imageCenter.x && indicatorCenter.y > imageCenter.y) {
        // 4th quardrant
        cv::rotate(image,image,cv::ROTATE_180);
    }

    // Display the image in a window
    cv::namedWindow("Auto Rotate Image");
    cv::moveWindow("Auto Rotate Image", 0, 0);
    cv::imshow("Auto Rotate Image", image);

    // Plot histogram
    plotHistogram(image,histogramType,saveOutputs);

    // Save Outputs 
    if (saveOutputs) {
        cv::imwrite("./output.png",image);
    }

    // Wait for a key press indefinitely
    cv::waitKey(0);

    // Close all OpenCV windows
    cv::destroyAllWindows();

    // std::cin.get();

    return 0;
}
