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
    scene_ = new GraphicScene(this);
    doc_controller_ = new DocController(this);
    ui->graphicsView->setScene(scene_);
    doc_controller_->SetScene(scene_);
    ui->dockWidget->SetScene(scene_);
    // Delete Shape Action
    connect(ui->actionDelete_Selection, &QAction::triggered, scene_, &GraphicScene::DeleteSelectedItem);
    connect(ui->menuButtons->ui->pushButton_Delete, &QPushButton::clicked, ui->actionDelete_Selection, &QAction::trigger);
    // Open File Action
    connect(ui->actionOpen, &QAction::triggered, doc_controller_, &DocController::OpenFile);
    connect(ui->menuButtons->ui->pushButton_Open, &QPushButton::clicked, ui->actionOpen, &QAction::trigger);
    // Undo Action
    connect(ui->actionUndo, &QAction::triggered, scene_, &GraphicScene::Undo);
    connect(ui->menuButtons->ui->pushButton_Undo, &QPushButton::clicked, ui->actionUndo, &QAction::trigger);
    // Redo Action
    connect(ui->actionRedo, &QAction::triggered, scene_, &GraphicScene::Redo);
    connect(ui->menuButtons->ui->pushButton_Redo, &QPushButton::clicked, ui->actionRedo, &QAction::trigger);
    // Save Action
    connect(ui->actionSave, &QAction::triggered, doc_controller_, &DocController::SaveFile);
    connect(ui->menuButtons->ui->pushButton_Save, &QPushButton::clicked, ui->actionSave, &QAction::trigger);
    // SaveAs Action
    connect(ui->actionSave_As, &QAction::triggered, doc_controller_, &DocController::SaveAs);
    connect(ui->menuButtons->ui->pushButton_SaveAs, &QPushButton::clicked, ui->actionSave_As, &QAction::trigger);
    // New File Action
    connect(ui->actionNew, &QAction::triggered, doc_controller_, &DocController::NewFile);
    connect(ui->menuButtons->ui->pushButton_New, &QPushButton::clicked, ui->actionNew, &QAction::trigger);
    // Copy Action
    connect(ui->actionCopy, &QAction::triggered, scene_, &GraphicScene::CopyAction);
    connect(ui->menuButtons->ui->pushButton_Copy, &QPushButton::clicked, ui->actionCopy, &QAction::trigger);
    // Cut Action
    connect(ui->actionCut, &QAction::triggered, scene_, &GraphicScene::CutAction);
    connect(ui->menuButtons->ui->pushButton_Cut, &QPushButton::clicked, ui->actionCut, &QAction::trigger);
    // Paste Action
    connect(ui->actionPaste, &QAction::triggered, scene_, &GraphicScene::PasteAction);
    connect(ui->menuButtons->ui->pushButton_Paste, &QPushButton::clicked, ui->actionPaste, &QAction::trigger);
    // Scene and visual canvas connection
    connect(scene_, &GraphicScene::CentreCanvasOn, ui->graphicsView, &CanvasWidget::CentreOn);
    // Display mouse position
    connect(ui->graphicsView, &CanvasWidget::MousePosition, this, &MainWindow::SetMousePosition);
    // Notify scene of tool change
    connect(ui->toolbarWidget, &ToolBarWidget::ToolChanged, scene_, &GraphicScene::SetToolType);
    // Notify property dock of selection change
    connect(scene_, &GraphicScene::SelectedShapesChanged, ui->dockWidget, &PropertiesDock::SetSelectedShapes);
    // Change main window title to signify changes
    connect(scene_->GetUndoStack(), &UndoStack::StackChanged, this, [=]() {
        std::string filename = doc_controller_->GetCurFilename();
        if(filename.empty()) filename = "New File";
        if(scene_->IsSaved()) {
            this->setWindowTitle(QString::fromStdString(filename));
        }
        else {
            this->setWindowTitle(QString::fromStdString("*" + filename));
        }
    });
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if(doc_controller_->DiscardChangesDialog()) {
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

void MainWindow::SetMousePosition(int x, int y)
{
    ui->label->setText(QString("X: %1,   Y: %2").arg(x).arg(y));
}
