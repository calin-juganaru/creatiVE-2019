FLAGS = -std=c++17 -lGL -lglut -lGLEW -lGLU -lglfw -O3
APP_NAME = test_app

build: main.o
	g++ $^ $(FLAGS) -o $(APP_NAME)

main.o: main.cpp
	g++ $^ $(FLAGS) -c

$(APP_NAME): build

run: $(APP_NAME)
	./$(APP_NAME)

clean:
	rm -f *.o $(APP_NAME)