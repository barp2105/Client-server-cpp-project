# Client server cpp project
 A C++ implementation of a multithreaded server and client for finding the shortest paths in an undirected graph using BFS algorithm.
Features

Multithreaded server handling multiple client connections
BFS implementation for shortest path finding
Server-side caching of the last 10 requests
Graph loading from CSV file
TCP/IP communication

Server
./a.out <filename> <port>
The server loads an undirected graph from a CSV file where each line represents an edge (space-separated vertex numbers). It listens for client connections and spawns a new thread for each client request.
Client
./a.out <ip> <port> <v1> <v2>
Connects to the server and requests the shortest path between vertices v1 and v2.
Build Instructions
Both client and server can be compiled using:
g++ *.cpp
Project Structure

/server - Server implementation files
/client - Client implementation files

Implementation Details

Uses TCP/IP sockets for communication
Implements thread-safe request handling
Maintains LRU cache for recent requests
Processes undirected graph data from CSV input
