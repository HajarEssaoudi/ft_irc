SRC = src/main.cpp \
      src/Server.cpp \
      src/Client.cpp \
      src/parsing.cpp \
	  src/utils.cpp \
	  src/AuthCmd.cpp

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98 -g

NAME = ircserv

OBJS = ${SRC:.cpp=.o}

RM = rm -f

%.o: %.cpp
	${CC} ${FLAGS} -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean re fclean