1: taskbarWindow.o draft.o AQI.o
	g++ taskbarWindow.o draft.o AQI.o -o 1 -mwindows -lgdi32 -lgdiplus -ld2d1 -ldwrite -LD:\programming\python\libs -lpython37
taskbarWindow.o: taskbarWindow.cpp
	g++ -c taskbarWindow.cpp
AQI.o: AQI.cpp
	g++ -c AQI.cpp -ID:\programming\python\include
draft.o: draft.cpp
	g++ -c draft.cpp
clean:
	@del *.o draft.exe