FLAGS = -std=c++17 -lGL -lglut -lGLEW -lGLU -lglfw -O3
APP_NAME = test_app
INCL_PATH = ./include

build: main.o 2D_text.o
	g++ $^ $(FLAGS) -o $(APP_NAME)

main.o: main.cpp
	g++ $^ $(FLAGS) -c

2D_text.o: $(INCL_PATH)/2D_text.cpp
	g++ $^ $(FLAGS) -c

$(APP_NAME): build

run: $(APP_NAME)
	./$(APP_NAME)

clean:
	rm -f *.o $(APP_NAME)