#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QString>

class PaintCanvas;

class MainWindow : public QMainWindow, public Ui::mainwindowClass
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    void setPointRenderButton();
    void setMeshRenderButton();
    void disableRenderButton();
    void iniStatusBar();
    void updateSatusBarCoord(QString &qstrX, QString &qstrY, QString &qstrZ);
    QString getQStringCurrentDir(){ return m_qstrCurrentDir; }

public slots:
    bool openModel();
    bool openSkeleton();
    bool saveSkeleton();
    void openRecentModel();
    void openRecentSkeleton();
    void setFrontLinesMode();
    void setPointsMode();
    void setAllLinesMode();
    void setSurfaceMode();
    void reset();
    void about();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    void readSettings();
    void writeSetttings();
    bool okToContinue();
    void setActions();
    bool loadObjectModelFile(const QString &qstrFileName);
    bool loadSkeletonFile(const QString &qstrSklName);
    bool saveSkeletonFile(const QString &qstrSklName);
    void setCurrentModel(const QString &modelName);
    void setCurrentSkeleton(const QString &sklName);
    void createRecentModels();
    void createRecentSkeletons();
    void updateRecentModelActions();
    void updateRecentSkeletonActions();
    QString strippedName(const QString &fullFileName);

    Ui::mainwindowClass *ui;
    PaintCanvas *m_pMainCanvas;
    QString m_qstrCurrentDir;
    QString m_qstrCurrentFile;

    static const int s_kMaxRecentFiles = 5;          
    QAction *m_recentModelActions[s_kMaxRecentFiles];    //list of recent models
    QAction *m_recentSkeletonActions[s_kMaxRecentFiles]; //list of recent skeletons
    QStringList m_recentModels; //list of models' name
    QStringList m_recentSkeletons; //list of skeletons' name

    std::string m_currentModel;
    std::string m_currentSkeleton;  

    QLabel *m_loadingStatusLabel;
    QLabel *m_labelCoordX;
    QLabel *m_labelCoordY;
    QLabel *m_labelCoordZ;
};

#endif // _MAINWINDOW_H_
