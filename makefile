markTool: main markTool.cpp
	g++ -std=c++11 markTool.cpp -I/usr/include/opencv /usr/lib/x86_64-linux-gnu/libopencv_core.so -lopencv_core /usr/lib/x86_64-linux-gnu/libopencv_highgui.so -lopencv_highgui /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so -lopencv_imgproc -o markTool


main: main.cpp
	g++  main.cpp -I/usr/include/opencv /usr/lib/x86_64-linux-gnu/libopencv_core.so -lopencv_core /usr/lib/x86_64-linux-gnu/libopencv_highgui.so -lopencv_highgui /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so -lopencv_imgproc -o main
