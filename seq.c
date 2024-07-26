#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <opencv2/opencv.hpp>

using namespace cv;

// Example number of images; adjust as needed
const int num_images = 10;

// Define image paths as arrays of strings
const char* input_sequentiel[num_images] = {
    "/home/yasser_jemli/syscalls/input",
    // Add paths for other images here
};
const char* output_sequentiel[num_images] = {
    "/home/yasser_jemli/syscalls/output/sequentiel",
    // Add paths for other images here
};

void process_image(const char* input_image, const char* output_image) {
    // Load the image
    Mat img = imread(input_image, IMREAD_COLOR);
    if (img.empty()) {
        fprintf(stderr, "Could not open or find the image %s\n", input_image);
        exit(EXIT_FAILURE);
    }

    // Convert to grayscale
    Mat gray_img;
    cvtColor(img, gray_img, COLOR_BGR2GRAY);

    // Save the grayscale image
    imwrite(output_image, gray_img);
}

int main() {
    pid_t pids[num_images];
    int status;
    for (int i = 0; i < num_images; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0) {
            // Child process
            process_image(input_sequentiel[i], output_sequentiel[i]);
            exit(EXIT_SUCCESS);
        }
    }

    // Parent process waits for all child processes to complete
    for (int i = 0; i < num_images; i++) {
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) {
            printf("Process %d finished with status %d\n", pids[i], WEXITSTATUS(status));
        } else {
            printf("Process %d did not finish correctly\n", pids[i]);
        }
    }

    return 0;
}
