#pragma once

#include <QMainWindow>
#include <QIntValidator>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool uygunmu(int satir,int sutun,int sayi);
    bool cozuldumu();
public slots:
    void yeniSudoku(int zorluk);
private slots:
    void on_hucreDegisti();
    void on_kontrolet_clicked();
    void on_yenisudoku_clicked();

private:
    Ui::MainWindow *ui;

    quint8 sudokuMatrisi[9][9];
    quint8 cozumMatrisi[9][9];

    bool isSafe(quint8 board[9][9],int row,int col, int num);
    bool solveSudoku(quint8 board[9][9]);
};
