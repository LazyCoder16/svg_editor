#include "mainwindow.h"
#include "canvaswidget.h"
#include "doccontroller.h"
#include "graphicscene.h"
#include "toolbarwidget.h"
#include "ui_mainwindow.h"
#include <QtCore/qobjectdefs.h>
#include <QtGui/qaction.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qpushbutton.h>
#include "menubuttonswidget.h"
#include "ui_menubuttonswidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new GraphicScene(this);
    docController = new DocController(this);
    ui->graphicsView->setScene(scene);

    /*// Setup menu item button click signals to action slots
    connect(ui->menuButtons->ui->pushButton_New, SIGNAL(clicked()), ui->actionNew, SLOT(trigger()));
    connect(ui->menuButtons->ui->pushButton_Copy, SIGNAL(clicked()), ui->actionCopy, SLOT(trigger()));
    connect(ui->menuButtons->ui->pushButton_Cut, SIGNAL(clicked()), ui->actionCut, SLOT(trigger()));
    connect(ui->menuButtons->ui->pushButton_Open, SIGNAL(clicked()), ui->actionOpen, SLOT(trigger()));
    connect(ui->menuButtons->ui->pushButton_Paste, SIGNAL(clicked()), ui->actionPaste, SLOT(trigger()));
    connect(ui->menuButtons->ui->pushButton_Redo, SIGNAL(clicked()), ui->actionRedo, SLOT(trigger()));
    connect(ui->menuButtons->ui->pushButton_Save, SIGNAL(clicked()), ui->actionSave, SLOT(trigger()));
    connect(ui->menuButtons->ui->pushButton_SaveAs, SIGNAL(clicked()), ui->actionSave_As, SLOT(trigger()));
    connect(ui->menuButtons->ui->pushButton_Undo, SIGNAL(clicked()), ui->actionUndo, SLOT(trigger()));*/

    // Delete Shape Action
    connect(ui->actionDelete_Selection, &QAction::triggered, scene, &GraphicScene::deleteSelectedItem);
    connect(ui->menuButtons->ui->pushButton_Delete, &QPushButton::clicked, ui->actionDelete_Selection, &QAction::trigger);
    // Open File Action
    connect(docController, &DocController::clearScene, scene, &GraphicScene::removeAllItems);
    connect(ui->menuButtons, &MenuButtonsWidget::file_loaded, docController, &DocController::load_file);
    connect(ui->actionOpen, &QAction::triggered, ui->menuButtons->ui->pushButton_Open, &QPushButton::click);
    // Undo Action
    connect(ui->actionUndo, &QAction::triggered, scene, &GraphicScene::undo);
    connect(ui->menuButtons->ui->pushButton_Undo, &QPushButton::clicked, ui->actionUndo, &QAction::trigger);
    // Redo Action
    connect(ui->actionRedo, &QAction::triggered, scene, &GraphicScene::redo);
    connect(ui->menuButtons->ui->pushButton_Redo, &QPushButton::clicked, ui->actionRedo, &QAction::trigger);

    connect(scene, &GraphicScene::centreCanvasOn, ui->graphicsView, &CanvasWidget::centreOn);
    connect(docController, &DocController::setViewportRect, scene, &GraphicScene::setViewportRect);
    connect(docController, &DocController::addShape, scene, &GraphicScene::addShape);

    connect(ui->menuButtons, &MenuButtonsWidget::buttonClicked, this, &MainWindow::buttonClicked);
    connect(ui->graphicsView, &CanvasWidget::mousePosition, this, &MainWindow::setMousePosition);

    connect(ui->menuButtons, &MenuButtonsWidget::file_loaded, docController, &DocController::load_file);
    connect(ui->toolbarWidget, &ToolBarWidget::toolChanged, scene, &GraphicScene::setToolType);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buttonClicked(const std::string& s)
{
    std::string message = s + " is clicked!!!";
    ui->label->setText(QString(s.c_str()));
}

void MainWindow::setMousePosition(int x, int y)
{
    ui->label->setText(QString("X: %1,   Y: %2").arg(x).arg(y));
}

/*
void MainWindow::on_actionNew_triggered()
{
    ui->label->setText("New is triggered!!!!");
}


void MainWindow::on_actionSave_triggered()
{
    ui->label->setText("Save is triggered!!!!");
}
*/
