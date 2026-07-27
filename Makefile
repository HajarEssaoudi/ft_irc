SRC =	src/main.cpp \
		src/Server.cpp \
		src/Client.cpp \
		src/Channel.cpp \
		src/utils.cpp \
		src/cmds/execCmd.cpp \
		src/cmds/helpers.cpp \
		src/cmds/invite.cpp \
		src/cmds/join.cpp \
		src/cmds/kick.cpp \
		src/cmds/mode.cpp \
		src/cmds/nick.cpp \
		src/cmds/part.cpp \
		src/cmds/pass.cpp \
		src/cmds/privmsg.cpp \
		src/cmds/topic.cpp \
		src/cmds/quit.cpp \
		src/cmds/user.cpp

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

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