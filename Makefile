default: Maze3D

%: %.cpp
	g++ -I. $< -o $@ AGL3Window.cpp ClosestPointTriangle.cpp -lepoxy -lGL -lglfw -std=c++17

clean:
	rm a.out *.o *~ Maze3D
