#include <opencv2/opencv.hpp>
#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <atomic>
#include <mutex>
#include <condition_variable>

class GifPlayer {
private:
    std::vector<cv::Mat> frames;
    double frameDelay;
    std::atomic<bool> isPaused{false};
    std::atomic<bool> shouldExit{false};
    std::mutex mtx;
    std::condition_variable cv;
    size_t currentFrame{0};

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append(static_cast<char*>(contents), size * nmemb);
        return size * nmemb;
    }

    std::string downloadImage(const std::string& url) {
        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
        std::string readBuffer;

        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }

        curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 30L);
        curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 0L); // Disable SSL verification for this example

        CURLcode res = curl_easy_perform(curl.get());
        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("curl_easy_perform() failed: ") + curl_easy_strerror(res));
        }

        return readBuffer;
    }

    double getFrameDelay(const cv::VideoCapture& cap) {
        double delay = cap.get(cv::CAP_PROP_FPS);
        return (delay > 0) ? 1000.0 / delay : 100.0;
    }

public:
    GifPlayer(const std::string& url) {
        std::cout << "Downloading image..." << std::endl;
        std::string imageData = downloadImage(url);

        std::vector<uchar> buffer(imageData.begin(), imageData.end());

        std::cout << "Processing GIF frames..." << std::endl;
        cv::VideoCapture cap;
        cap.open(buffer);

        if (!cap.isOpened()) {
            throw std::runtime_error("Could not open the GIF.");
        }

        cv::Mat frame;
        while (cap.read(frame)) {
            frames.push_back(frame.clone());
        }

        if (frames.empty()) {
            throw std::runtime_error("No frames were read from the GIF.");
        }

        frameDelay = getFrameDelay(cap);
    }

    void play() {
        cv::namedWindow("Skull Art by haydiroket", cv::WINDOW_NORMAL);
        cv::resizeWindow("Skull Art by haydiroket", frames[0].cols, frames[0].rows);

        std::cout << "Displaying GIF. Press 'ESC' to exit, 'SPACE' to pause/resume." << std::endl;

        std::thread renderThread([this]() {
            while (!shouldExit) {
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this]() { return !isPaused || shouldExit; });
                }

                if (shouldExit) break;

                cv::imshow("Skull Art by haydiroket", frames[currentFrame]);
                currentFrame = (currentFrame + 1) % frames.size();

                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(frameDelay)));
            }
        });

        while (true) {
            int key = cv::waitKey(1);
            if (key == 27) { // ESC key
                shouldExit = true;
                cv.notify_one();
                break;
            } else if (key == 32) { // SPACE key
                isPaused = !isPaused;
                cv.notify_one();
            }
        }

        renderThread.join();
        cv::destroyAllWindows();
    }
};

int main() {
    try {
        const std::string imageUrl = "https://media0.giphy.com/media/v1.Y2lkPTc5MGI3NjExZGsydGJjdHN0cGhpNXZnenY3anY3bmx1ejNvbmtocncwZjNkdWVqcSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/12hjJ7tjzvlQ08/giphy.gif";
        GifPlayer player(imageUrl);
        player.play();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
