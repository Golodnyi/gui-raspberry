#include <string>
using namespace std;

extern int gui_init(int argc, char *argv[]);
int PIN;
int SPEED;
string PROGRAM_PATH;
int main(int argc, char *argv[]) { gui_init(argc, argv); }
