executable_name = output

full: build run

fullclean: clean build run

build:
		g++ -o $(executable_name) *.cpp \
		    -lpthread \
		    -lsfml-graphics \
			-lsfml-window \
			-lsfml-system

run:
		./$(executable_name)

clean:
		rm -f ./$(executable_name)