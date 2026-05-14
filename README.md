# IRC Server

A custom Internet Relay Chat (IRC) server developed in **C++98**, built from scratch using **non-blocking sockets** and **poll()** for handling multiple clients simultaneously.

This project recreates the core behavior of an IRC server, allowing clients to connect, authenticate, join channels, exchange private messages, and manage channel permissions.

---

## Features

### Networking
- TCP socket server
- Non-blocking I/O
- Multiplexing with `poll()`
- Multiple clients support
- Graceful server shutdown (`Ctrl + C`)

### Authentication
- Password authentication (`PASS`)
- Nickname registration (`NICK`)
- User registration (`USER`)

### IRC Commands Implemented

| Command | Description |
|----------|-------------|
| `PASS` | Authenticate using the server password |
| `NICK` | Set or change nickname |
| `USER` | Register username |
| `JOIN` | Join a channel |
| `PART` | Leave a channel |
| `PRIVMSG` | Send private or channel messages |
| `KICK` | Remove a user from a channel |
| `INVITE` | Invite users to a channel |
| `TOPIC` | View or change channel topic |
| `MODE` | Configure channel modes |
| `QUIT` | Disconnect from server |
| `PING` | Connection keepalive |
| `PONG` | Response to ping |

---

## Technologies Used

- **C++98**
- **BSD Sockets API**
- **poll()**
- **Non-blocking sockets**
- **TCP/IP networking**

---

## Project Structure

```txt
.
├── includes/
├── srcs/
│   ├── Server.cpp
│   ├── Client.cpp
│   ├── Channel.cpp
│   ├── Parsing.cpp
│   └── Commands/
├── Makefile
└── ircserv
Compilation

Compile the project using:

make

Clean object files:

make clean

Remove binaries and objects:

make fclean

Recompile everything:

make re
Usage

Run the server with:

./ircserv <port> <password>

Example:

./ircserv 6667 mypassword

Where:

<port> → port where the server will listen
<password> → required password for client authentication
Testing

The server can be tested using:

Netcat (nc)

Connect to the server:

nc localhost 6667

Register a user:

PASS mypassword
NICK user1
USER user1 0 * :realname

Join a channel:

JOIN #general

Send a message:

PRIVMSG #general :Hello IRC!
HexChat

Connect using a graphical IRC client:

Open HexChat
Add a new network
Set:
Server: localhost/6667
Password: yourpassword
Connect to the server

Example channel:

#general
Server Architecture

The server uses a single-threaded event-driven architecture powered by poll().

Workflow
The server socket listens for incoming connections.
New clients are accepted and set to non-blocking mode.
Each client socket is registered inside a pollfd vector.
poll() waits for:
incoming connections (POLLIN)
incoming messages
pending outgoing messages (POLLOUT)
Commands are parsed and dispatched.
Responses are buffered and sent asynchronously.

This architecture allows handling multiple clients without threads.

Channel Management

Clients can:

Join channels
Leave channels
Invite users
Kick members
Set channel topics
Change channel modes
Error Handling

The server handles:

Invalid nicknames
Wrong passwords
Unknown commands
Missing parameters
Duplicate nicknames
Re-registration attempts
Client disconnections
IRC Protocol

The implementation follows the behavior of the IRC protocol (RFC 1459) for supported commands.

Authors

Developed as part of the 42 School IRC project.