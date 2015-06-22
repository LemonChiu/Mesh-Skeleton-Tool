#include "mainwindow.h"

#include "paint_canvas.h"
#include "mesh.h"
#include "object.h"
#include "file_io.h"
#include "stop_watch.h"

#include <QSettings>
#include <QFileDialog>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QCloseEvent>
#include <QMessageBox>

MainWindow::MainWindow()
{
    setupUi(this);
    setWindowTitle("Mesh Skeleton Tool");
    setAcceptDrops(true);
    setMinimumWidth(1024);
    m_pMainCanvas = new PaintCanvas(this);
    setCentralWidget(m_pMainCanvas);
    setActions();
    iniStatusBar();
    readSettings();
}

MainWindow::~MainWindow()
{
    writeSetttings();
    delete m_pMainCanvas;
    m_pMainCanvas = NULL;
}

bool MainWindow::openModel()
{
    QString qstrFileName = QFileDialog::getOpenFileName(this,
        tr("Open model from file"), m_qstrCurrentDir,
        tr("3D Model (*.obj *.ply)\n"
        "All files (*.*)"));

    if (qstrFileName.isEmpty()) {
        return false;
    }

    if (loadObjectModelFile(qstrFileName)){
        return true;
    } else { 
        return false;
    }
}

bool MainWindow::openSkeleton()
{
    QString qstrFileName = QFileDialog::getOpenFileName(this,
        tr("Open skeleton from file"), m_qstrCurrentDir,
        tr("3D Skeleton (*.cg *.out)\n"     
        "All files (*.*)"));

    if (qstrFileName.isEmpty()) {
        return false;
    }

    if (loadSkeletonFile(qstrFileName)) {
        return true;
    } else { 
        return false;
    }
}

bool MainWindow::saveSkeleton()
{
    QString qstrFileName = QFileDialog::getSaveFileName(this,
        tr("Save skeleton to file"), m_qstrCurrentDir,
        tr("Skeleton files (*.cg)\n"
        "Skeleton files (*.out)\n"
        "All files (*.*)"));

    if (qstrFileName.isEmpty()) {
        return false;
    }

    if (saveSkeletonFile(qstrFileName)) {
        return true;
    } else {
        return false;
    }
}

void MainWindow::readSettings()
{
    QSettings qSettings("SIAT-VCC", "MeshSkeleton");
    m_qstrCurrentDir = qSettings.value("currentDirectory").toString();  
    m_recentModels = qSettings.value("recentModels").toStringList();
    m_recentSkeletons = qSettings.value("recentSkeletons").toStringList();

    updateRecentModelActions();
    updateRecentSkeletonActions();
}

void MainWindow::writeSetttings()
{
    QSettings qSettings("SIAT-VCC", "MeshSkeleton");
    qSettings.setValue("currentDirectory", m_qstrCurrentDir);
    qSettings.setValue("recentModels", m_recentModels);
    qSettings.setValue("recentSkeletons", m_recentSkeletons);
}

bool MainWindow::okToContinue()
{
    return true;
}

void MainWindow::setActions()
{
    //Menubar Toolbar
    connect(actionOpen_Model, SIGNAL(triggered()), this, SLOT(openModel()));
    connect(actionOpen_Skeleton, SIGNAL(triggered()), this, SLOT(openSkeleton()));
    connect(actionSave_Skeleton, SIGNAL(triggered()), this, SLOT(saveSkeleton()));
    connect(actionReset, SIGNAL(triggered()), this, SLOT(reset()));

    connect(actionFrontLinesMode, SIGNAL(triggered()), this, SLOT(setFrontLinesMode()));
    connect(actionPointsMode, SIGNAL(triggered()), this, SLOT(setPointsMode()));
    connect(actionAllLinesMode, SIGNAL(triggered()), this, SLOT(setAllLinesMode()));
    connect(actionSurfaceMode, SIGNAL(triggered()), this, SLOT(setSurfaceMode()));
    connect(actionBackground, SIGNAL(triggered()), m_pMainCanvas, SLOT(changeBackgroundColor()));

    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

    //Edit Groupbox
    connect(addNodeRadioButton, SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(add_a_node(bool)));
    connect(deleteNodeRadioButton, SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(delete_a_node(bool)));
    connect(addBoneRadioButton, SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(add_a_bone(bool)));
    connect(deleteBoneRadioButton, SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(delete_a_bone(bool)));
    connect(sceneManipulationRadioButton, SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(scene_manipulation(bool)));

    //View Groupbox
    connect(widthSpinBox, SIGNAL(valueChanged(int)), m_pMainCanvas, SLOT(changePointWidth(int)));
    connect(raduisDoubleSpinBox, SIGNAL(valueChanged(double)), m_pMainCanvas, SLOT(changeNodeRaduis(double)));
    connect(redDoubleSpinBox, SIGNAL(valueChanged(double)), m_pMainCanvas, SLOT(changeMeshRedColor(double)));
    connect(greenDoubleSpinBox, SIGNAL(valueChanged(double)), m_pMainCanvas, SLOT(changeMeshGreenColor(double)));
    connect(blueDoubleSpinBox, SIGNAL(valueChanged(double)), m_pMainCanvas, SLOT(changeMeshBlueColor(double)));

    // Display Option
    connect(showModelCheckBox, SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(set_show_object(bool)));
    connect(showSkeletonCheckBox, SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(set_show_skl_bone(bool)));
    connect(showNodeCheckBox, SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(set_show_skl_node(bool)));
    connect(showAxisCheckBox,SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(set_show_axis(bool)));
    connect(showNormalCheckBox,SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(set_show_normal(bool)));

    //Algorithm Groupbox
    connect(centerModeRadioButton, SIGNAL(toggled(bool)), m_pMainCanvas, SLOT(set_center_mode(bool)));
    
    //Recent files
    createRecentModels();
    createRecentSkeletons();
}

// update list and title
void MainWindow::setCurrentModel(const QString &modelName)
{
    m_qstrCurrentFile = modelName;
    m_qstrCurrentDir = modelName.left(modelName.lastIndexOf("/"));
    setWindowModified(false);
    QString shownName = "Untitled";

    if (!m_qstrCurrentFile.isEmpty()) {
        shownName = strippedName(m_qstrCurrentFile);
        m_recentModels.removeAll(m_qstrCurrentFile);
        m_recentModels.prepend(m_qstrCurrentFile);
        updateRecentModelActions();
    }
    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Mesh Skeleton Tool")));

    //transfer to standard string
    m_currentModel = modelName.toStdString();
}

// set and update
void MainWindow::setCurrentSkeleton(const QString &sklName)
{
    m_qstrCurrentFile = sklName;
    m_qstrCurrentDir = sklName.left(sklName.lastIndexOf("/"));
    setWindowModified(false);
    QString shownName = "Untitled";

    if (!m_qstrCurrentFile.isEmpty()) {
        shownName = strippedName(m_qstrCurrentFile);
        m_recentSkeletons.removeAll(m_qstrCurrentFile);
        m_recentSkeletons.prepend(m_qstrCurrentFile);
        updateRecentSkeletonActions();
    }

    //transfer to standard string
    m_currentSkeleton = sklName.toStdString();
}

bool MainWindow::loadObjectModelFile(const QString &qstrFileName)
{
    Ui_mainwindowClass::statusBar->showMessage(tr("Loading model..."), 200);
    ObjectType type;
    Object *object = FileIO::readObject(qstrFileName.toStdString(), type);
    
    if (object == NULL || type == OBJECT_UNKOWN_TYPE) { 
        return false;
    } else {
        if (type == OBJECT_MESH_TYPE) {
            // set available render mode
            setMeshRenderButton();
        }       
        if (type == OBJECT_POINTSET_TYPE) {
            // in point mode
            setPointRenderButton();
        }
        m_pMainCanvas->setModel(object);
        m_pMainCanvas->update();
        setCurrentModel(qstrFileName);
        return true;
    }
}

bool MainWindow::loadSkeletonFile(const QString &qstrSklName)
{
    Ui_mainwindowClass::statusBar->showMessage(tr("Loading skeleton..."), 200);
    Skeleton *skl = FileIO::readSkeleton(qstrSklName.toStdString());
    if (skl == NULL) {
        return false;
    } else {
        m_pMainCanvas->setSkl(skl);
        m_pMainCanvas->update();
        setCurrentSkeleton(qstrSklName);
        return true;
    }
}

bool MainWindow::saveSkeletonFile(const QString &qstrSklName) 
{
    Skeleton *skl = m_pMainCanvas->get_skeleton();
    Normalizer *normalizer = m_pMainCanvas->get_normalizer();

    if (skl == NULL || normalizer == NULL) {
        return false;
    }

    FileIO::saveSkeleton(skl, qstrSklName.toStdString(), normalizer);
    setCurrentSkeleton(qstrSklName);

    return true;
}

void MainWindow::createRecentModels()
{
    //make recent model visible and add connection
    for (int i = 0; i < s_kMaxRecentFiles; ++i) {
        m_recentModelActions[i] = new QAction(this);
        m_recentModelActions[i]->setVisible(false);
        connect(m_recentModelActions[i], SIGNAL(triggered()), this, SLOT(openRecentModel()));
    }

    //add to menu
    for (size_t i = 0; i < s_kMaxRecentFiles; ++i) {
        menuRecent_Models -> addAction(m_recentModelActions[i]);
    }
}

void MainWindow::createRecentSkeletons()
{
    //make recent skeleton visible and add connection
    for (int i = 0; i < s_kMaxRecentFiles; ++i) {
        m_recentSkeletonActions[i] = new QAction(this);
        m_recentSkeletonActions[i]->setVisible(false);
        connect(m_recentSkeletonActions[i], SIGNAL(triggered()), this, SLOT(openRecentSkeleton()));
    }

    //add to menu
    for (size_t i = 0; i < s_kMaxRecentFiles; ++i) {
        menuRecent_Skeletons -> addAction(m_recentSkeletonActions[i]);
    }
}

void MainWindow::openRecentModel()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        loadObjectModelFile(action->data().toString());
    }
}

void MainWindow::openRecentSkeleton()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        loadSkeletonFile(action->data().toString());
    }
}

void MainWindow::updateRecentModelActions()
{
    // clear all
    QMutableStringListIterator it(m_recentModels);
    while (it.hasNext()) {
        if (!QFile::exists(it.next())) {
            it.remove();
        }
    }

    for (int j = 0; j < s_kMaxRecentFiles; ++j) {
        if (j < m_recentModels.count()) {
            QString text = tr("&%1 %2").arg(j + 1).arg((m_recentModels[j]));
            m_recentModelActions[j]->setText(text);
            m_recentModelActions[j]->setData(m_recentModels[j]);
            m_recentModelActions[j]->setVisible(true);
        } else {
            m_recentModelActions[j]->setVisible(false);
        }
    }
}

void MainWindow::updateRecentSkeletonActions()
{
    // clear all
    QMutableStringListIterator it(m_recentSkeletons);
    while (it.hasNext()) {
        if (!QFile::exists(it.next()))
            it.remove();
    }

    for (int j = 0; j < s_kMaxRecentFiles; ++j) {
        if (j < m_recentSkeletons.count()) {
            QString text = tr("&%1 %2").arg(j + 1).arg(strippedName(m_recentSkeletons[j]));
            m_recentSkeletonActions[j]->setText(text);
            m_recentSkeletonActions[j]->setData(m_recentSkeletons[j]);
            m_recentSkeletonActions[j]->setVisible(true);
        } else {
            m_recentSkeletonActions[j]->setVisible(false);
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {

        QList<QUrl> urlList = mimeData->urls();
        // set the first URL as local file path
        QString fileDir = urlList.at(0).toLocalFile();

        if (fileDir.endsWith("obj") || fileDir.endsWith("ply")) {
            // obj or ply model files
            std::cout << "Open file: "<< fileDir.toStdString() << std::endl;
            loadObjectModelFile(fileDir);
            return;
        } else if (fileDir.endsWith("cg") || fileDir.endsWith("skl") || fileDir.endsWith("out")) {
            // cg, skl, out skeleton files
            std::cout << "Open file: "<< fileDir.toStdString() << std::endl;
            loadSkeletonFile(fileDir);
            return;
        }
    }
}

void MainWindow::setPointsMode(){
    global::g_meshDrawMode = POINTSMODE;
    m_pMainCanvas->update();
}

void MainWindow::setFrontLinesMode()
{
    global::g_meshDrawMode = FRONTLINESMODE;
    m_pMainCanvas->update();
}

void MainWindow::setAllLinesMode()
{
    global::g_meshDrawMode = ALLLINESMODE;
    m_pMainCanvas->update();
}

void MainWindow::setSurfaceMode()
{
    global::g_meshDrawMode = SURFACEMODE;
    m_pMainCanvas->update();
}

void MainWindow::setPointRenderButton() 
{
    actionPointsMode->setEnabled(true);
    actionAllLinesMode->setEnabled(false);
    actionFrontLinesMode->setEnabled(false);
    actionSurfaceMode->setEnabled(false);
}

void MainWindow::setMeshRenderButton()
{
    actionAllLinesMode->setEnabled(true);
    actionFrontLinesMode->setEnabled(true);
    actionSurfaceMode->setEnabled(true);
    actionPointsMode->setEnabled(true);
}

void MainWindow::disableRenderButton()
{
    actionPointsMode->setEnabled(false);
    actionAllLinesMode->setEnabled(false);
    actionFrontLinesMode->setEnabled(false);
    actionSurfaceMode->setEnabled(false);
}

void MainWindow::iniStatusBar()
{
    // 180px space
    m_loadingStatusLabel = new QLabel;
    m_loadingStatusLabel->setMinimumWidth(180);
    
    m_labelCoordX = new QLabel;
    m_labelCoordX->setMinimumWidth(100);

    m_labelCoordY = new QLabel;
    m_labelCoordY->setMinimumWidth(100);

    m_labelCoordZ = new QLabel;
    m_labelCoordZ->setMinimumWidth(100);

    Ui_mainwindowClass::statusBar->addWidget(m_loadingStatusLabel);
    Ui_mainwindowClass::statusBar->addWidget(m_labelCoordX);
    Ui_mainwindowClass::statusBar->addWidget(m_labelCoordY);
    Ui_mainwindowClass::statusBar->addWidget(m_labelCoordZ);
}

void MainWindow::updateSatusBarCoord(QString &qstr_x, QString &qstr_y, QString &qstr_z)
{
    m_labelCoordX->setText("X: " + qstr_x);
    m_labelCoordY->setText("Y: " + qstr_y);
    m_labelCoordZ->setText("Z: " + qstr_z);
}

// reset button
void MainWindow::reset()
{
   actionAllLinesMode->setEnabled(false);
   disableRenderButton();
   m_pMainCanvas->resetClear();
}

// about button
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Mesh Skeleton Tool"),
        tr("<h2>Mesh Skeleton Tool 1.0</h2>"
        "<p>This program uses:<br />"
        "Qt Version 4.8.4 <br />"
        "Boost Version 1.53 <br />"
        "CGAL Version 4.1 <br />"
        "QGLViewer Version 2.3.17</p>"
        "<p>Copyright &copy; 2014 LemonC.</p>"));
}

// get file name from path
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

// confirm to close
void MainWindow::closeEvent(QCloseEvent * event)
{
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("Mesh Skeleton Tool"),
        QString(tr("Do you really want to exit the program?")),
        QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::No) {
        event->ignore();  //continue to run
    } else if (button == QMessageBox::Yes) {
        writeSetttings();
        event->accept();  //exit
    }
}