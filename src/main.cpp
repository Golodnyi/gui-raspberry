#include <iostream>
#include <bitset>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zconf.h>

using namespace std;

extern int gui_init(int argc, char *argv[], vector<char> temp_vector);
extern int flex(int argc, char *argv[],  vector<char> temp_vector);

int main(int argc, char *argv[]) {
    vector<char> temp_vector;
    gui_init(argc, argv, temp_vector);
    flex(argc, argv, temp_vector);
}
