CC = g++
CPPFLAGS = -std=c++11
TARGET = progress_bar
OBJ = main.o progress_bar.o

all : progress_bar

progress_bar : $(OBJ)
	@echo "<***Linking***> $@"
	@$(CC) $(CPPFLAGS) $(OBJ) -o $(TARGET)

main.o : main.cpp
	@echo "<**Compiling**> $@"
	@$(CC) $(CPPFLAGS) -c $^

progress_bar.o : progress_bar.cpp
	@echo "<**Compiling**> $@"
	@$(CC) $(CPPFLAGS) -c $^

clean :
	@rm -rf progress_bar $(OBJ)
