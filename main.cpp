#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <QApplication>
#include <QTextEdit>
#include "data.hpp"
#include <time.h>


/*void sleepcp(int milliseconds) // Cross-platform sleep function
{
    clock_t time_end;
    time_end = clock() + milliseconds * CLOCKS_PER_SEC/1000;
    while (clock() < time_end)
    {
    }
}*/

int main(int argc, char* argv[]){
    try{
        Data fileData(argv[1],argv[2]);
        QApplication app(argc, argv);
        fileData.obtain_results(app);
        //app.exec();
        //sleepcp(5000);
    }
    catch(...){
        std::cerr << "You have to provide a valid address for the data as argument and output address of csv\n";
    }
}
