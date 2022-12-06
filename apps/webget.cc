#include "socket.hh"
#include "util.hh"

#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace std;

void get_URL(const string &host, const string &path) {
    // Your code here.
    Address addr(host, "http");
    // cout << "ip:port: " << addr.to_string() << endl;
    TCPSocket tcp_scocket;
    tcp_scocket.connect(addr);

    stringstream ss;
    ss << "GET " << path << " HTTP/1.1\r\n";
    ss << "Host: cs144.keithw.org\r\n";
    ss << "Connection: close\r\n";
    ss << "\r\n";

    string send_message = ss.str();
    // cout << send_message << endl;
    size_t n = tcp_scocket.write(send_message, send_message.size());
    if (n != send_message.size()) {
        cerr << "call write error, expect size: " << send_message.size() << ", actual size: " << n;
        return;
    }
    string message;
    while (!tcp_scocket.eof()) {
        tcp_scocket.read(message, 1024);
        cout << message;
    }

    // You will need to connect to the "http" service on
    // the computer whose name is in the "host" string,
    // then request the URL path given in the "path" string.

    // Then you'll need to print out everything the server sends back,
    // (not just one call to read() -- everything) until you reach
    // the "eof" (end of file).

    // cerr << "Function called: get_URL(" << host << ", " << path << ").\n";
    // cerr << "Warning: get_URL() has not been implemented yet.\n";
}

int main(int argc, char *argv[]) {
    try {
        if (argc <= 0) {
            abort();  // For sticklers: don't try to access argv[0] if argc <= 0.
        }

        // The program takes two command-line arguments: the hostname and "path" part of the URL.
        // Print the usage message unless there are these two arguments (plus the program name
        // itself, so arg count = 3 in total).
        if (argc != 3) {
            cerr << "Usage: " << argv[0] << " HOST PATH\n";
            cerr << "\tExample: " << argv[0] << " stanford.edu /class/cs144\n";
            return EXIT_FAILURE;
        }

        // Get the command-line arguments.
        const string host = argv[1];
        const string path = argv[2];

        // Call the student-written function.
        get_URL(host, path);
    } catch (const exception &e) {
        cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
