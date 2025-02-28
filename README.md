# UDP Chat Application

## Overview
This project is a simple UDP-based chat application that allows multiple clients to communicate with each other through a server. 
It is implemented using Windows Sockets (Winsock) and supports basic message logging and error handling.

## Features
- UDP-based socket communication
- Multiple clients can join and send messages
- Logging system for debugging and error tracking
- Client exit command

## Components
### Server
The server listens for incoming messages from clients and broadcasts them to all connected clients.

### Client
Clients send messages to the server and receive responses. They can also exit the chat by sending the "exit" command.

## How It Works
1. The server binds to a specified port and waits for incoming messages.
2. Clients connect to the server and send messages.
3. The server receives the messages and logs them.
4. The server then relays messages to other clients.
5. Clients can type "exit" to disconnect from the server.

## Logging
The application includes a logging system that records system messages, client interactions, and errors. Stored in a /Logs/Timestamp_Log.txt file.

## Future Improvements
- Implement a proper client list to track connected clients.
- Add support for usernames or identifiers.
- Improve message broadcasting to all clients.
- Enhance error handling and robustness.
