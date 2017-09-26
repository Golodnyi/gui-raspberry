#include <QSound>
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;
extern string PROGRAM_PATH;
void *warning(void *arg)
{
    string path = "aplay " + PROGRAM_PATH + "warning.wav";
    const char *cstr = path.c_str();
    system(cstr);
}

void *danger(void *arg)
{
    string path = "aplay " + PROGRAM_PATH + "danger.wav";
    const char *cstr = path.c_str();
    system(cstr);
    /**cout << "play danger: " << PROGRAM_PATH + "signal.wav" << endl;
    QSound sound(QString::fromStdString(PROGRAM_PATH + "signal.wav"));
    sound.setLoops(3);
    sound.play();**/
}