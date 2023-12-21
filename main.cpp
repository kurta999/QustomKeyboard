
#include "mainwindow.h"

#include <QApplication>
#include <boost/algorithm/string.hpp>

#include "src/ApplicationBase.hpp"

MainWindow* g_MainWindow = nullptr;
ApplicationBase* g_app;

int main(int argc, char *argv[])
{
    std::string str = "f a sz";
    boost::algorithm::replace_all(str, " ", "_");
    QCoreApplication::addLibraryPath("C:\\Qt\\Qt-6.5.0\\plugins\\platforms\\");
    QApplication a(argc, argv);
    
    g_app = new ApplicationBase();
    g_app->Init();

    MainWindow w;
    w.show();


    g_MainWindow = &w;
    int ret = a.exec();

    delete g_app;
    g_app = nullptr;
    return ret;
}
