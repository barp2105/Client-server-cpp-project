/*
Final cpp project by:
Bar Partush - 315431221  , 
Amit Kalaf  - 208311654
*/


// Includes required libraries and dependencies for server-side functionality

#include "../std_lib_facilities.h"
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <queue>
#include <list>
#include <utility>
#include <thread>

// Graph type definition using a map of integer lists
typedef map<int, list<int>> Graph;

// Memory structure to cache paths found during searches
list<pair<pair<int,int>,list<int>>> memory;

// Forward declarations for functions used in graph searching
list<int> BFSsearch(int start, int end, Graph graph);
list<int> CacheMemoryCheck(Graph &nGraph, int start, int end);

// Reads a graph from a file and constructs a graph data structure
Graph readGraph(const string& file) {
    Graph readedGraph;
    ifstream f(file);   // File stream for reading
    string lineToRead;  // String to hold each line of the file
    
    // Parse each line to extract graph edges
    while (getline(f, lineToRead)) {
        istringstream splitToWords(lineToRead); // Stream to split the line into integers
        int v1, v2;                            // Vertex identifiers
        while (splitToWords >> v1 >> v2) {
            readedGraph[v1].push_back(v2);
            readedGraph[v2].push_back(v1);
        }
    }
    return readedGraph;     // Return the constructed graph
}


// Checks if a path is stored in cache memory and returns it; otherwise computes it using BFS
list<int> CacheMemoryCheck(Graph &nGraph, int start, int end) {
    
    list<int> path;     // List to store the path from start to end

    // Search in cache for the requested path
    for (auto &memoryChek: memory) {
        if (memoryChek.first.first == start && memoryChek.first.second== end) {
            return memoryChek.second;       // Return cached path if found
        }
    }
    
    // Compute the path using BFS if not found in cache
    path = BFSsearch(start,end,nGraph);

    // Prepare the cache entry
    pair<int,int> vectors={start,end};
    pair<pair<int,int>,list<int>> temp={vectors,path};
    
    // Manage the cache size: remove the oldest entry if cache is full
    if (memory.size() >= 10) {
        memory.pop_front();      // Remove oldest entry
        memory.push_back(temp); // Add new path to cache
    } else {
        memory.push_back(temp);
    }
    return path;        // Return the computed or cached path
}


// Handles requests from a client by computing the shortest path in the graph

void clientRequests(int client_socket, const Graph &nGraph) {
    // Buffer to store incoming message from client.
    char arr[1024];
    memset(arr, 0, sizeof(arr));    // Initialize the buffer to zero

    // Read message from client socket.
    ssize_t msg = read(client_socket, arr, sizeof(arr)-1);
    arr[sizeof(arr)-1]='\0';    // Null-terminate the string

    // Check if message reading was successful
    if (msg < 0) {
        perror("Failed to read the message");   // Log error if read fails
    }
    // Parse the start and end points from the received message
    istringstream iss(arr);
    int start, end;
    iss >> start >> end;
    
    // Get the shortest path between the start and end nodes from the graph
    list<int> path = CacheMemoryCheck(const_cast<Graph &>(nGraph), start, end);

    string Response;        // String to store the response message
    
    // Prepare the response based on whether a path was found
    if (path.empty()) {
        Response = "path not found";    // No path found response
    } else {
        // Concatenate all vertices in the path into a response string
        for (auto v: path) {
            Response += to_string(v) + " ";
        }
    }
    // Send the response back to the client
    write(client_socket, Response.c_str(), Response.size()-1);
    close(client_socket);
}

// Performs Breadth-First Search to find the shortest path between two vertices

list<int> BFSsearch(int start, int end, Graph graph) {
    list<int> path; // List to store the path from start to end
    map<int, int> prevV; // Map to store the predecessor of each vertex
    map<int, bool> visitedV; // Map to keep track of visited vertices
    queue<int> childList; // Queue to manage the BFS frontier

    prevV[start] = -1; // Set the predecessor of the start vertex as -1 (no predecessor)

    // Initialize all vertices as unvisited
    for (auto vertice : graph) {
        visitedV[vertice.first] = false;
    }

    // Start BFS from the start vertex
    childList.push(start);
    visitedV[start] = true;
    while (!childList.empty()) {
        int vertice = childList.front();
        childList.pop();
        // Check if the current vertex is the destination
        if (vertice == end) {
            // Construct the path by tracing predecessors from the end to the start
            for (int prevCheck = end; prevCheck != -1; prevCheck = prevV[prevCheck]) {
                path.push_front(prevCheck);
            }
            return path;    // Return the found path
        }

        // Explore each adjacent vertex
        for (int next : graph[vertice]) {
            if (visitedV[next] == false) { // Check if the vertex has not been visited
                childList.push(next);
                visitedV[next] = true;
                prevV[next] = vertice; // Set the current vertex as predecessor
            }
        }
    }
     // Handle the case where no path is found
    if (path.empty()) {
        perror("cannot find a path between those 2 vertices");  // Log an error
    }
    return path;    // Return the path (empty if no path found)
}

// Main function: initializes the server and handles incoming connections

int main(int argc, char* argv[]) {
    // Check for correct usage and arguments
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <graph_file> <port>" << endl;
        return 1;   // Exit if the required arguments are not provided
    }

    // Read the graph from the specified file
    Graph graph = readGraph(argv[1]);

    // Create a TCP socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("Error creating socket");
        return 1;   // Exit if socket creation fails
    }

    // Set up the socket address structure
    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");    // Bind to all local addresses
    addr.sin_port = htons(atoi(argv[2]));        // Convert port number from string to network byte order

    // Bind the socket to the address
    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Error binding socket");
        close(fd);
        return 1;   // Exit if binding fails
    }

    // Listen on the socket for incoming connections
    if (listen(fd, 5) < 0) {
        perror("Error listening on socket");
        close(fd);
        return 1;   // Exit if listening setup fails
    }

    sockaddr_in client_addr;    // Client address that will connect
    socklen_t client_addr_len = sizeof(client_addr);    // Size of client address
    int fd2;        // Socket file descriptor for the connected client

    // Server loop: accept and handle connections indefinitely
    while (true) {
        // Accept a new connection
        if ((fd2 = accept(fd, (struct sockaddr*)&client_addr, &client_addr_len)) < 0) {
            perror("Error accept - problem");
            continue;   // Continue in the loop if accepting fails
        }

        // Spawn a new thread to handle the client request independently
        thread(clientRequests, fd2, ref(graph)).detach();   // Detach thread to run independently
    }

    return 0;   // Should never reach this point
}