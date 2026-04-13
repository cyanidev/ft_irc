# program name
NAME = ircserv

# compiler
CC = c++

CFLAGS = -Werror -Wextra -Wall -std=c++98 #-g3

INC = -I includes

SRC = \
      srcs/main.cpp \
	  srcs/Server.cpp \
	  srcs/Channel.cpp \
	  srcs/Tokenizer.cpp \
	  srcs/handleOperation.cpp \
	  srcs/Parsing.cpp \
	  srcs/Client.cpp

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
