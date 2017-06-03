#include <iostream>
#include "Socket.h"

using std::cerr;
using std::cout;

inline parseCommand(int &argc, char **argv)
{
    if (argc > 1)
    {
        showHelp();
        exit(0);
    }
}
void showHelp();

int main(int argc, char **argv)
{
    tmc::Socket s;
    parseCommand(argc, argv);

    s.init("21");
    s.listen();
    while(true)
    {
        s.accept();
    }
}

void showHelp()
{
    cout <<
"\n\
Tamce Simple File Transmit Protocal(TSFTP)\n\
         Server Side Program\n\
\n\
Usage:\n\
\n\
tsftpd
";

}
