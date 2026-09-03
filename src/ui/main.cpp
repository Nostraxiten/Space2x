#include <QApplication>
#include <QIcon>
#include <space2x/core/Engine.h>
#include "MainWindow.h"
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Space2X");
    app.setOrganizationName("Space2X");
    app.setWindowIcon(QIcon(":/resources/app_icon.png"));

    auto engine = space2x::core::Engine::create();
    if (!engine) {
        std::cerr << "Fatal Error: Failed to initialize Space2X core engine.\n";
        return 1;
    }

    space2x::ui::MainWindow mainWindow(std::move(engine));
    mainWindow.setWindowIcon(QIcon(":/resources/app_icon.png"));
    mainWindow.show();

    return app.exec();
}

