#include <QSound>
#include <iostream>

using namespace std;
extern string PROGRAM_PATH;
void *warning(void *arg)
{
    cout << "play warning: " << PROGRAM_PATH + "signal.wav" << endl;
    QSound sound(QString::fromStdString(PROGRAM_PATH + "signal.wav"));
    sound.setLoops(1);
    sound.play();
}

void *danger(void *arg)
{
    cout << "play danger: " << PROGRAM_PATH + "signal.wav" << endl;
    QSound sound(QString::fromStdString(PROGRAM_PATH + "signal.wav"));
    sound.setLoops(3);
    sound.play();
}