# dicom-image-reader
## Setup (Installation for Windows)
### CMake
- go to https://cmake.org/download/
- download the `cmake-3.30.3-windows-x86_64.msi`
- install it

### OpenCV
- refer to https://docs.opencv.org/4.x/d3/d52/tutorial_windows_install.html#tutorial_windows_install_prebuilt
- go to https://sourceforge.net/projects/opencvlibrary/
- download the `opencv-4.9.0-windows.exe`
- run it and extract it desired path (Chose C drive)
- ~~open up command prompt and `setx OpenCV_DIR  C:\opencv\build`~~
    - ~~Can refer back to https://docs.opencv.org/4.x/d3/d52/tutorial_windows_install.html#tutorial_windows_install_path~~
- ~~add `C:\opencv\build\x64\vc16\bin` and `C:\opencv\build\x64\vc16\lib` to PATH~~


## Compile (CMake commands)
- Make sure the `set(OpenCV_DIR "C:/opencv/build")` in the `CMakeLists.txt` is set to the correct OpenCV path previously extracted
- Assume the command line current is at the project `build`
    - `cmake "../CMakeLists.txt"`
        - configure the cmake project
    - `cmake --build .`
        - build the Debug version exe
        - will see the `dicom-image-reader.exe` in the `build/Debug` if build success
    - `cmake --build . --config Release`
        - build the Release version exe
        - will see the `dicom-image-reader.exe` in the `build/Release` if build success

## Run (Arguments)
- `--input="XXX"`
    - The input image path (default would be `image.png` in the `dicom-image-reader.exe` directory)
    - Eg. `dicom-image-reader.exe --input="./image.png"`
- `--save_outputs`
    - Save the histogram  and the auto rotated image as png
    - Eg. `dicom-image-reader.exe --input="./image.png" --save_outputs`
    - The saved images will be in the same directory as the `dicom-image-reader.exe`
- `--histogram_type=X`
    - The histogram chart type
        - default is bar chart
        - 1 is line chart
    - Eg. `dicom-image-reader.exe --input="./image.png" --histogram_type=1`