CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

RM = rm -f

NAME = ircserv
NAME_BONUS = ircserv_bonus

# ==========================
# Mandatory
# ==========================

MANDATORY_DIR = mandatory

MANDATORY_SRC = \
	$(MANDATORY_DIR)/src/main.cpp \
	$(MANDATORY_DIR)/src/Server.cpp \
	$(MANDATORY_DIR)/src/Client.cpp \
	$(MANDATORY_DIR)/src/Channel.cpp \
	$(MANDATORY_DIR)/src/utils.cpp \
	$(MANDATORY_DIR)/src/cmds/execCmd.cpp \
	$(MANDATORY_DIR)/src/cmds/helpers.cpp \
	$(MANDATORY_DIR)/src/cmds/invite.cpp \
	$(MANDATORY_DIR)/src/cmds/join.cpp \
	$(MANDATORY_DIR)/src/cmds/kick.cpp \
	$(MANDATORY_DIR)/src/cmds/mode.cpp \
	$(MANDATORY_DIR)/src/cmds/nick.cpp \
	$(MANDATORY_DIR)/src/cmds/part.cpp \
	$(MANDATORY_DIR)/src/cmds/pass.cpp \
	$(MANDATORY_DIR)/src/cmds/privmsg.cpp \
	$(MANDATORY_DIR)/src/cmds/topic.cpp \
	$(MANDATORY_DIR)/src/cmds/quit.cpp \
	$(MANDATORY_DIR)/src/cmds/user.cpp \
	$(MANDATORY_DIR)/src/cmds/ping.cpp \
	$(MANDATORY_DIR)/src/cmds/whois.cpp

MANDATORY_OBJ = $(MANDATORY_SRC:.cpp=.o)

# ==========================
# Bonus
# ==========================

BONUS_DIR = Bonus

BONUS_SRC = \
	$(BONUS_DIR)/src/main.cpp \
	$(BONUS_DIR)/src/Server.cpp \
	$(BONUS_DIR)/src/Client.cpp \
	$(BONUS_DIR)/src/Channel.cpp \
	$(BONUS_DIR)/src/utils.cpp \
	$(BONUS_DIR)/src/cmds/execCmd.cpp \
	$(BONUS_DIR)/src/cmds/helpers.cpp \
	$(BONUS_DIR)/src/cmds/invite.cpp \
	$(BONUS_DIR)/src/cmds/join.cpp \
	$(BONUS_DIR)/src/cmds/kick.cpp \
	$(BONUS_DIR)/src/cmds/mode.cpp \
	$(BONUS_DIR)/src/cmds/nick.cpp \
	$(BONUS_DIR)/src/cmds/part.cpp \
	$(BONUS_DIR)/src/cmds/pass.cpp \
	$(BONUS_DIR)/src/cmds/privmsg.cpp \
	$(BONUS_DIR)/src/cmds/topic.cpp \
	$(BONUS_DIR)/src/cmds/quit.cpp \
	$(BONUS_DIR)/src/cmds/user.cpp \
	$(BONUS_DIR)/src/cmds/ping.cpp \
	$(BONUS_DIR)/src/cmds/whois.cpp

BONUS_OBJ = $(BONUS_SRC:.cpp=.o)

# ==========================
# Rules
# ==========================

all: mandatory

mandatory: $(MANDATORY_OBJ)
	$(CC) $(CFLAGS) $(MANDATORY_OBJ) -o $(NAME)

bonus: $(BONUS_OBJ)
	$(CC) $(CFLAGS) $(BONUS_OBJ) -o $(NAME_BONUS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(MANDATORY_OBJ) $(BONUS_OBJ)

fclean: clean
	$(RM) $(NAME) $(NAME_BONUS)

re: fclean all

.PHONY: all mandatory bonus clean fclean re