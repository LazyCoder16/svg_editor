#include "mainwindow.h"
#include "canvaswidget.h"
#include "doccontroller.h"
#include "graphicscene.h"
#include "propertiesdock.h"
#include "toolbarwidget.h"
#include "ui_mainwindow.h"
#include <QtCore/qobjectdefs.h>
#include <QtGui/qaction.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qpushbutton.h>
#include "menubuttonswidget.h"
#include "ui_menubuttonswidget.h"
#include "undostack.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("New File");
    scene = new GraphicScene(this);
    docController = new DocController(this);
    ui->graphicsView->setScene(scene);
    docController->setScene(scene);
    ui->dockWidget->setScene(scene);
    // Delete Shape Action
    connect(ui->actionDelete_Selection, &QAction::triggered, scene, &GraphicScene::deleteSelectedItem);
    connect(ui->menuButtons->ui->pushButton_Delete, &QPushButton::clicked, ui->actionDelete_Selection, &QAction::trigger);
    // Open File Action
    connect(ui->actionOpen, &QAction::triggered, docController, &DocController::openFile);
    connect(ui->menuButtons->ui->pushButton_Open, &QPushButton::clicked, ui->actionOpen, &QAction::trigger);
    // Undo Action
    connect(ui->actionUndo, &QAction::triggered, scene, &GraphicScene::undo);
    connect(ui->menuButtons->ui->pushButton_Undo, &QPushButton::clicked, ui->actionUndo, &QAction::trigger);
    // Redo Action
    connect(ui->actionRedo, &QAction::triggered, scene, &GraphicScene::redo);
    connect(ui->menuButtons->ui->pushButton_Redo, &QPushButton::clicked, ui->actionRedo, &QAction::trigger);
    // Save Action
    connect(ui->actionSave, &QAction::triggered, docController, &DocController::saveFile);
    connect(ui->menuButtons->ui->pushButton_Save, &QPushButton::clicked, ui->actionSave, &QAction::trigger);
    // SaveAs Action
    connect(ui->actionSave_As, &QAction::triggered, docController, &DocController::saveAs);
    connect(ui->menuButtons->ui->pushButton_SaveAs, &QPushButton::clicked, ui->actionSave_As, &QAction::trigger);
    // New File Action
    connect(ui->actionNew, &QAction::triggered, docController, &DocController::newFile);
    connect(ui->menuButtons->ui->pushButton_New, &QPushButton::clicked, ui->actionNew, &QAction::trigger);
    // Copy Action
    connect(ui->actionCopy, &QAction::triggered, scene, &GraphicScene::copyAction);
    connect(ui->menuButtons->ui->pushButton_Copy, &QPushButton::clicked, ui->actionCopy, &QAction::trigger);
    // Cut Action
    connect(ui->actionCut, &QAction::triggered, scene, &GraphicScene::cutAction);
    connect(ui->menuButtons->ui->pushButton_Cut, &QPushButton::clicked, ui->actionCut, &QAction::trigger);
    // Paste Action
    connect(ui->actionPaste, &QAction::triggered, scene, &GraphicScene::pasteAction);
    connect(ui->menuButtons->ui->pushButton_Paste, &QPushButton::clicked, ui->actionPaste, &QAction::trigger);
    // Scene and visual canvas connection
    connect(scene, &GraphicScene::centreCanvasOn, ui->graphicsView, &CanvasWidget::centreOn);
    // Display mouse position
    connect(ui->graphicsView, &CanvasWidget::mousePosition, this, &MainWindow::setMousePosition);
    // Notify scene of tool change
    connect(ui->toolbarWidget, &ToolBarWidget::toolChanged, scene, &GraphicScene::setToolType);
    // Notify property dock of selection change
    connect(scene, &GraphicScene::selectedShapesChanged, ui->dockWidget, &PropertiesDock::setSelectedShapes);
    // Change main window title to signify changes
    connect(&scene->undoStack, &UndoStack::stackChanged, this, [=]() {
        std::string filename = docController->curFilename;
        if(filename.empty()) filename = "New File";
        if(scene->undoStack.isClean()) {
            this->setWindowTitle(QString::fromStdString(filename));
        }
        else {
            this->setWindowTitle(QString::fromStdString("*" + filename));
        }
    });
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if(docController->discardChangesDialog()) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMousePosition(int x, int y)
{
    ui->label->setText(QString("X: %1,   Y: %2").arg(x).arg(y));
}
