#include <iostream>
#include <time.h>
#include "CSMACD.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    bool full_log;
    bool repeat;
    do
    {
        srand(time(0));
        int n;
        cout << "Enter number of stations:\n";
        cin >> n;

        cout << "Print full log? [y/n]\n";
        char ans;
        cin >> ans;
        full_log = ans == 'y';

        CSMACD cd(n, full_log);
        cd.reproduce_sending();

        cout << "Repeat solution?[y/n]\n";
        cin >> ans;
        repeat = ans == 'y';

    } while(repeat);
    return 0;
}