To run the following commands in the Developer Command Prompt for VS to compile the project:

 cd CableDefectDetection
 mkdir build && cd build
 cmake ..
 cmake --build . --config Debug

This generates the exe file and then in powershell run:

 cd Debug
 .\CableMeasurement.exe

The output images to Exercise 1,2 and 3 are saved in the following paths:
cd \CableDefectDetection\Exercise1_output
cd \CableDefectDetection\Exercise2_output
cd \CableDefectDetection\Exercise3_output