1: taskbarWindow.o draft.o
	g++ taskbarWindow.o draft.o -o 1 -lgdi32 -lgdiplus -ld2d1 -ldwrite
taskbarWindow.o: taskbarWindow.cpp
	g++ -c taskbarWindow.cpp
draft.o: draft.cpp
	g++ -c draft.cpp
clean:
	@del *.o draft.exe