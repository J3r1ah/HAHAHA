#include <opencv2/opencv.hpp>
#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <chrono>
#include <thread>

// Callback function for writing data from curl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to download image data using libcurl
std::string downloadImage(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return "";
        }
    }
    return readBuffer;
}

int main() {
    // URL of the GIF image
    std::string imageUrl = "https://media0.giphy.com/media/v1.Y2lkPTc5MGI3NjExZGsydGJjdHN0cGhpNXZnenY3anY3bmx1ejNvbmtocncwZjNkdWVqcSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/12hjJ7tjzvlQ08/giphy.gif";

    // Download the image data
    std::string imageData = downloadImage(imageUrl);
    if (imageData.empty()) {
        std::cerr << "Error: Could not download the image." << std::endl;
        return -1;
    }

    // Convert image data to a vector of bytes
    std::vector<uchar> buffer(imageData.begin(), imageData.end());

    // Read the GIF frames
    std::vector<cv::Mat> frames;
    cv::VideoCapture cap;
    cap.open(buffer);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open the GIF." << std::endl;
        return -1;
    }

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        frames.push_back(frame.clone());
    }

    if (frames.empty()) {
        std::cerr << "Error: No frames were read from the GIF." << std::endl;
        return -1;
    }

    // Create a window to display the image
    cv::namedWindow("Skull Art by haydiroket", cv::WINDOW_NORMAL);

    // Resize the window to match the image dimensions
    cv::resizeWindow("Skull Art by haydiroket", frames[0].cols, frames[0].rows);

    // Display the animated GIF
    int frameIndex = 0;
    while (true) {
        cv::imshow("Skull Art by haydiroket", frames[frameIndex]);

        // Wait for 100ms or key press
        int key = cv::waitKey(100);
        if (key == 27) // ESC key
            break;

        frameIndex = (frameIndex + 1) % frames.size();
    }

    // Close the window
    cv::destroyAllWindows();

    return 0;
}
