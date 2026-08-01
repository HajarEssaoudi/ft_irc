CC      = c++
CFLAGS  = -Wall -Wextra -Werror -std=c++98
RM      = rm -f
NAME    = ircserv

COMMON_SRC = \
	src/main.cpp \
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
	src/cmds/ping.cpp \
	src/cmds/quit.cpp \
	src/cmds/topic.cpp \
	src/cmds/user.cpp \
	src/cmds/whois.cpp

MANDATORY_SRC = $(COMMON_SRC) src/cmds/privmsg.cpp
MANDATORY_OBJ = $(MANDATORY_SRC:.cpp=.o)

BONUS_SRC     = $(COMMON_SRC) bonus/privmsg.cpp
BONUS_OBJ     = $(BONUS_SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(MANDATORY_OBJ)
	$(CC) $(CFLAGS) $(MANDATORY_OBJ) -o $(NAME)

bonus: $(BONUS_OBJ)
	$(CC) $(CFLAGS) $(BONUS_OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(MANDATORY_OBJ) $(BONUS_OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re