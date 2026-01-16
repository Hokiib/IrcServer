SRCS =  $(addprefix src/, main.cpp Server.cpp Client.cpp Channel.cpp CommandHandler.cpp)
OBJECTS = $(SRCS:.cpp=.o)
DEPS = $(OBJECTS:.o=.d)
INCLUDES = ./includes

CC = c++
FLAGS = -Wall -Werror -Wextra -std=c++98 -MMD -MP

NAME = ircserv
PASSWORD = password

all: $(NAME)

start: re
	./$(NAME) 6667 $(PASSWORD) 

debug: FLAGS += -DDEBUG -g3
debug: re
	valgrind --track-fds=yes ./$(NAME) 6667 $(PASSWORD)

$(NAME): $(OBJECTS)
	$(CC) $(FLAGS) -I $(INCLUDES) $(OBJECTS) -o $(NAME)

%.o: %.cpp
	$(CC) $(FLAGS) -I $(INCLUDES) -c $< -o $@ -g3

-include $(DEPS)

clean:
	rm -rf $(OBJECTS) $(DEPS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re start debug
