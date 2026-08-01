*This project has been created as part of the 42 curriculum by zerrosaa, hes-saou, lalfidi.*

# ft_irc

## Description

ft_irc is a fully functional IRC (Internet Relay Chat) server written in C++98. IRC is a text-based communication protocol created in 1988, where clients connect to a server and exchange messages in real time through channels or private messages.

The goal of this project is to implement a non-blocking IRC server capable of handling multiple simultaneous client connections using a single `poll()` call, without forking or using threads. The server is compatible with standard IRC clients such as irssi and HexChat.

### Features

- Multiple simultaneous client connections via `poll()`
- Full IRC authentication flow: `PASS`, `NICK`, `USER`
- Channel management: `JOIN`, `PART`, `TOPIC`, `INVITE`, `KICK`
- Channel modes: `+i` (invite-only), `+t` (topic restricted), `+k` (password), `+o` (operator), `+l` (user limit)
- Private messaging: `PRIVMSG`
- Operator commands: `KICK`, `MODE`, `INVITE`, `TOPIC`
- Graceful client disconnection: `QUIT`
- Signal handling: `SIGINT`, `SIGQUIT`
- No memory leaks

### Bonus

- **IRC Bot** — A `JokeBot` that responds to `joke` and `help` commands via `PRIVMSG`
- **DCC** — File transfer detection

---

## Instructions

### Requirements

- C++ compiler supporting C++98 (`c++`)
- `make`
- A Unix-based system (Linux / macOS)

### Compilation

```bash
# Compile the mandatory part
make

# Compile with bonus (bot + DCC)
make bonus

# Clean object files
make clean

# Full clean (objects + binary)
make fclean

# Recompile everything
make re
```

### Execution

```bash
./ircserv <port> <password>
```

**Example:**

```bash
./ircserv 6667 mypassword
```

- `port` must be between 1025 and 65535
- `password` cannot be empty

### Testing with netcat

```bash
nc -C localhost 6667
PASS mypassword
NICK Alice
USER alice 0 * :Alice Dupont
JOIN #general
PRIVMSG #general :Hello everyone!
QUIT :bye
```

### Testing with irssi

```bash
irssi
/connect localhost 6667 mypassword Alice
/join #general
/msg Bob Hello!
/quit bye
```

### Testing the bot (bonus)

```bash
/msg JokeBot joke
/msg JokeBot help
```

---

## Project Structure

```
ft_irc/
├── Makefile
├── README.md
├── includes/
│   ├── Server.hpp
│   ├── Client.hpp
│   ├── Channel.hpp
│   └── utils.hpp
├── src/
│   ├── main.cpp
│   ├── Server.cpp
│   ├── Client.cpp
│   ├── Channel.cpp
│   ├── utils.cpp
│   └── cmds/
│       ├── execCmd.cpp
│       ├── privmsg.cpp
│       ├── join.cpp
│       ├── kick.cpp
│       ├── mode.cpp
│       ├── invite.cpp
│       ├── topic.cpp
│       ├── part.cpp
│       ├── nick.cpp
│       ├── pass.cpp
│       ├── user.cpp
│       ├── ping.cpp
│       ├── quit.cpp
│       └── whois.cpp
└── bonus/
    └── privmsg.cpp
```

### Team responsibilities

| Member | Responsibility |
|--------|---------------|
| Zainab Errossafi | Network layer: socket, poll(), recv(), Client class |
| [Coéquipière 2] | Parser, authentication (PASS, NICK, USER), channels |
| [Coéquipière 3] | Commands (PRIVMSG, KICK, MODE, INVITE, TOPIC), bot |

---

## Resources

### Documentation

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459) — The original IRC specification
- [RFC 2812 — IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812) — Updated IRC client protocol
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — Comprehensive guide to sockets in C
- [Linux man pages — poll(2)](https://man7.org/linux/man-pages/man2/poll.2.html) — Official poll() documentation
- [Linux man pages — socket(2)](https://man7.org/linux/man-pages/man2/socket.2.html) — Official socket() documentation

### Articles & Tutorials

- [Socket Programming in C](https://www.geeksforgeeks.org/socket-programming-cc/) — GeeksforGeeks tutorial
- [IRC Numerics Reference](https://www.alien.net.au/irc/irc2numerics.html) — List of all IRC error and reply codes

### AI Usage

AI was used throughout this project for the following purposes:

- **Understanding concepts** — Detailed explanations of sockets, file descriptors, TCP, poll(), and the IRC protocol
- **Code review** — Reviewing Server.cpp and Client.cpp for correctness and best practices
- **Architecture guidance** — Advising on the separation of responsibilities between team members and the project structure
- **README** — Drafting this README file

AI was used as a learning tool. All code was written, understood, and validated by the team members.
