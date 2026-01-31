# Cable Defect Detection

This project implements a computer vision inspection pipeline for detecting and classifying defects in cables.
It was developed as a technical exercise to demonstrate image processing, feature extraction, and rule-based defect classification using C++.

The system processes images of cables and performs:

Cable diameter estimation

Defect detection

Defect classification (e.g. pin-holes, cuts, scratches)

# Project Structure
```
CableDefectDetection/
├── src/
│   └── Main.cpp                # Main application source code
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project documentation
├── *.bmp                       # Input test images
├── Exercise1_output/           # Diameter estimation results
├── Exercise2_output/           # Defect detection results
├── Exercise3_output/           # Defect classification results
├── Challenge_answers.pdf       # Written answers to theoretical questions
└── build/                      # Generated build files (Visual Studio / CMake)
```

# Input Images

The project operates on grayscale or color images of cables, for example:

Pin-Hole.bmp

Cut.bmp

Scratches.bmp

Pin-Hole_and_cut.bmp

Each image may contain one or multiple defect types.

# Processing Pipeline
1. Cable Diameter Estimation (Exercise 1)

The first stage extracts the cable body from the background and estimates its diameter by analyzing the cable’s cross-section.

Output examples (saved in Exercise1_output/):

diameteroutput_Pin-Hole.jpg

diameteroutput_Cut.jpg

These images visualize:

Detected cable edges

Measured diameter overlayed on the image

2. Defect Detection (Exercise 2)

In this stage, the algorithm identifies anomalies on the cable surface by comparing local intensity and texture variations against the nominal cable appearance.

Detected defects are highlighted directly on the image.

Output examples (saved in Exercise2_output/):

defectoutput1_Scratches.jpg

defectoutput1_Pin-Hole_and_cut.jpg

3. Defect Classification (Exercise 3)

Detected defects are classified into categories such as:

Pin-hole

Cut

Scratch

Classification is based on geometric and appearance features such as:

Area

Shape

Aspect ratio

Local contrast

Output examples (saved in Exercise3_output/):

classifyoutput1_Pin-Hole.jpg

classifyoutput1_Cut.jpg

classifyoutput1_Scratches.jpg

Each output image shows the detected defect along with its predicted label.

# Build & Run Instructions
Requirements

C++ compiler (Visual Studio / GCC / Clang)

CMake

OpenCV (assumed by the implementation)

Build (example using CMake)
mkdir build
cd build
cmake ..
cmake --build .

Run

After building, execute:

CableDefectDetection.exe


The program processes the provided .bmp images and writes the results to the corresponding Exercise*_output folders.




