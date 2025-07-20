# program name
NAME = ircserv

# compiler
CC = c++

CFLAGS = -Werror -Wextra -Wall -std=c++98 #-g3

INC = -I includes

SRC = \
      srcs/main.cpp

OBJ = $(SRC:srcs/%.cpp=objs/%.o)

all: $(NAME)
	
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o  $@ $(OBJ) $(INC)

objs/%.o: srcs/%.cpp
	@mkdir -p objs
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
