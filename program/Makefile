executable_name = output

full: build run

fullclean: clean build run

build:
		g++ -o $(executable_name) *.cpp \
			-std=c++17 \
			-lpthread \
			-lsfml-graphics \
			-lsfml-window \
			-lsfml-system

run:
		./$(executable_name)

clean:
		rm -f ./$(executable_name)