#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // URL of the GIF image
    std::string imageUrl = "https://media0.giphy.com/media/v1.Y2lkPTc5MGI3NjExZGsydGJjdHN0cGhpNXZnenY3anY3bmx1ejNvbmtocncwZjNkdWVqcSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/12hjJ7tjzvlQ08/giphy.gif";

    // Read the image from the URL
    cv::Mat image = cv::imread(imageUrl, cv::IMREAD_COLOR);

    // Check if the image was successfully loaded
    if (image.empty()) {
        std::cerr << "Error: Could not load the image." << std::endl;
        return -1;
    }

    // Create a window to display the image
    cv::namedWindow("Skull Art by haydiroket", cv::WINDOW_NORMAL);

    // Resize the window to match the image dimensions
    cv::resizeWindow("Skull Art by haydiroket", image.cols, image.rows);

    // Display the image
    cv::imshow("Skull Art by haydiroket", image);

    // Wait for a key press
    cv::waitKey(0);

    // Close the window
    cv::destroyAllWindows();

    return 0;
}
