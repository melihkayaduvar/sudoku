#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <ctime>   // srand ve time için
#include <cstdlib> // rand için
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Rastgele sayı üretecini programın başında SADECE BİR KEZ belirliyoruz.
    srand(time(nullptr));


    for(int i =0;i<9;++i){
        for(int j = 0;j<9;++j){
            QString widgetAdi = QString("m_%1x%2").arg(i).arg(j);
            QLineEdit *hucre = this->findChild<QLineEdit *>(widgetAdi);
            if(hucre){
                // --- YENİ EKLENECEK KISIM ---
                hucre->setMaxLength(1);
                // Sadece 1-9 arası girişlere izin veren bir validator oluştur.
                // 'this' parametresi, validator'ın MainWindow ile birlikte silinmesini sağlar.
                QValidator *validator = new QIntValidator(1, 9, this);
                // Validator'ı hücreye ata.
                hucre->setValidator(validator);
                // --- BİTTİ ---
                connect(hucre, &QLineEdit::textChanged, this,&MainWindow::on_hucreDegisti);
            }
        }
    }

    ui->zorlukcb->setStyleSheet( // ComboBox'ınızın adı "comboBox" ise. Değilse adını değiştirin.
        "QComboBox {"
        "    background-color: #2E2E2E;"      // Kutu arkaplanı
        "    color: white;"                   // Kutu metin rengi
        "    border: 1px solid #555555;"       // Kenarlık
        "    border-radius: 5px;"
        "    padding-left: 10px;"              // İçten boşluk
        "}"
        // Açılır menünün stili
        "QComboBox QAbstractItemView {"
        "    background-color: #2E2E2E;"      // Menü arkaplanı
        "    color: white;"                   // Menüdeki yazıların rengi
        "    selection-background-color: #6A0DAD;" // SEÇİLİ ÖĞENİN arkaplanı (Mor renk)
        "    selection-color: white;"           // SEÇİLİ ÖĞENİN yazı rengi
        "    border: 1px solid #555555;"
        "}"
        // Aşağı ok butonunun stili (opsiyonel ama şık durur)
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        );

    // Program açıldığında direkt olarak yeni bir oyun başlatalım.
    yeniSudoku(1); // 1 = Kolay
}

MainWindow::~MainWindow(){

    delete ui;
}

// Bu fonksiyon, kullanıcının yaptığı bir hamlenin geçerli olup olmadığını kontrol eder.
bool MainWindow::uygunmu(int satir, int sutun, int sayi) {
    // sudokuTahtasi üzerinde isSafe kontrolü yapıyoruz.
    return isSafe(this->sudokuMatrisi, satir, sutun, sayi);
}
// isSafe fonksiyonunu MainWindow'un bir metodu yapıyoruz.
bool MainWindow::isSafe(quint8 board[9][9], int row, int col, int num) {
    for (int x = 0; x < 9; x++) if (board[row][x] == num) return false;
    for (int x = 0; x < 9; x++) if (board[x][col] == num) return false;
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) if (board[i + startRow][j + startCol] == num) return false;
    return true;
}
// solveSudoku fonksiyonunu MainWindow'un bir metodu yapıyoruz.
bool MainWindow::solveSudoku(quint8 board[9][9]) {
    int row, col;
    bool isEmpty = false;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            if (board[row][col] == 0) {
                isEmpty = true; break;
            }
        }
        if (isEmpty) break;
    }
    if (!isEmpty) return true;

    // Her seferinde farklı bir tahta üretmek için sayıları rastgele sırada deneyelim.
    int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    // Basit bir karıştırma (shuffle)
    for(int i=0; i<9; ++i) {
        int r = rand() % 9;
        int temp = nums[i];
        nums[i] = nums[r];
        nums[r] = temp;
    }

    for (int i = 0; i < 9; i++) {
        int num = nums[i];
        if (isSafe(board, row, col, num)) {
            board[row][col] = num;
            if (solveSudoku(board)) return true;
            board[row][col] = 0; // Backtrack
        }
    }
    return false;
}
// mainwindow.cpp

void MainWindow::yeniSudoku(int zorluk)
{
    // ... (Mevcut kodunun ilk kısmı aynı kalıyor) ...
    memset(cozumMatrisi, 0, sizeof(cozumMatrisi));
    solveSudoku(cozumMatrisi);
    memcpy(sudokuMatrisi, cozumMatrisi, sizeof(cozumMatrisi));

    int hucresil = 45;
    if (zorluk == 1) hucresil = 1;
    if (zorluk == 3) hucresil = 50;

    int silinenHucresayisi = 0;
    while (silinenHucresayisi < hucresil) {
        int satir = rand() % 9;
        int sutun = rand() % 9;
        if (sudokuMatrisi[satir][sutun] != 0) {
            sudokuMatrisi[satir][sutun] = 0;
            silinenHucresayisi++;
        }
    }
    // Koyu tema için temel stilimizi tanımlıyoruz.
    QString baseStyle = "background-color: #2E2E2E;" // Koyu gri arkaplan
                        "border: 1px solid #555555;"   // Biraz daha açık gri kenarlık
                        "border-radius: 5px;"
                        "text-align: center;"         // Sayıları ortala
                        "font-size: 16pt;"            // Yazı boyutunu büyüt
                        "font-weight: bold;";         // Yazıları kalın yap

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            QString widgetAdi = QString("m_%1x%2").arg(i).arg(j);
            QLineEdit *hucre = this->findChild<QLineEdit *>(widgetAdi);

            if (hucre) {
                int sayi = sudokuMatrisi[i][j];
                if (sayi != 0) {
                    hucre->setText(QString::number(sayi));
                    hucre->setReadOnly(true);
                    // Başlangıç sayıları için beyaz renk
                    hucre->setStyleSheet(baseStyle + "color: white;");
                } else {
                    hucre->clear();
                    hucre->setReadOnly(false);
                    // Kullanıcının gireceği sayılar için açık mavi renk
                    hucre->setStyleSheet(baseStyle + "color: #55E0FF;");
                }
            }
        }
    }
}

void MainWindow::on_hucreDegisti()
{
    QLineEdit *hucre = qobject_cast<QLineEdit*>(sender());
    if (!hucre) return;

    // --- BU FONKSİYONUN TAMAMINI GÜNCELLEYİN ---

    QString ad = hucre->objectName();
    ad.remove(0, 2);
    QStringList parts = ad.split('x');
    int satir = parts[0].toInt();
    int sutun = parts[1].toInt();
    QString text = hucre->text();

    if (text == "0") {
        hucre->clear();
        return;
    }

    QString baseStyle = "background-color: #2E2E2E;"
                        "border: 1px solid #555555;"
                        "border-radius: 5px;"
                        "text-align: center;"
                        "font-size: 16pt;"
                        "font-weight: bold;";

    if (text.isEmpty()) {
        sudokuMatrisi[satir][sutun] = 0;
        hucre->setStyleSheet(baseStyle + "color: #55E0FF;");
        return;
    }

    int sayi = text.toInt();
    sudokuMatrisi[satir][sutun] = sayi;

    sudokuMatrisi[satir][sutun] = 0; // Geçici olarak sil
    bool guvenliMi = isSafe(sudokuMatrisi, satir, sutun, sayi);
    sudokuMatrisi[satir][sutun] = sayi; // Kontrolden sonra değeri hemen geri koy!

    // 3. Kontrol sonucunu SADECE rengi belirlemek için kullan.
    if (guvenliMi) {
        // Hamle geçerli: Açık mavi
        hucre->setStyleSheet(baseStyle + "color: #55E0FF;");
    } else {
        // Hamle geçersiz: Kırmızı
        hucre->setStyleSheet(baseStyle + "color: #FF5555;");
    }
}

// Bu fonksiyon, tahtada hiç boş hücre kalıp kalmadığını kontrol eder.
// Daha gelişmiş bir versiyon, tüm tahtanın doğruluğunu da kontrol edebilir.
bool MainWindow::cozuldumu() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            // Eğer bir hücre boşsa VEYA kullanıcının girdiği cevap doğru değilse,
            // bulmaca çözülmemiştir.
            if (sudokuMatrisi[i][j] == 0 || sudokuMatrisi[i][j] != cozumMatrisi[i][j]) {
                return false;
            }
        }
    }
    // Eğer döngü bittiyse ve hiç hata bulunmadıysa, çözülmüştür.
    return true;
}

void MainWindow::on_yenisudoku_clicked()
{
    quint8 x;
    if(ui->zorlukcb->currentText()=="Kolay"){
        x=1;
    }else if (ui->zorlukcb->currentText()=="Orta"){
        x=2;
    }else if (ui->zorlukcb->currentText()=="Zor"){
        x=3;
    }
    yeniSudoku(x);
}


void MainWindow::on_kontrolet_clicked()
{
    // Temel stilimizi burada da tanımlayalım ki renkleri değiştirirken
    // arkaplan, kenarlık gibi diğer stilleri kaybetmeyelim.
    QString baseStyle = "background-color: #2E2E2E;"
                        "border: 1px solid #555555;"
                        "border-radius: 5px;"
                        "text-align: center;"
                        "font-size: 16pt;"
                        "font-weight: bold;";

    bool tumuDogru = true; // Tahtanın tamamen doğru olup olmadığını takip etmek için bir bayrak.

    // Tahtadaki 81 hücrenin tamamını gez.
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            // İlgili QLineEdit'i bul.
            QString widgetAdi = QString("m_%1x%2").arg(i).arg(j);
            QLineEdit *hucre = this->findChild<QLineEdit *>(widgetAdi);

            if (!hucre) continue; // Hücre bulunamazsa atla.

            // Sadece kullanıcının doldurduğu hücreleri kontrol et.
            // Başlangıçta gelen, değiştirilemeyen (read-only) hücreleri es geç.
            if (!hucre->isReadOnly()) {
                int kullaniciSayisi = sudokuMatrisi[i][j];
                int dogruSayi = cozumMatrisi[i][j];

                // Eğer hücre boş değilse VE kullanıcının girdiği sayı doğru değilse...
                if (kullaniciSayisi != 0 && kullaniciSayisi != dogruSayi) {
                    tumuDogru = false; // Bir tane bile hata bulsak bayrağı indiriyoruz.
                    // Bu hücreyi KIRMIZI yap.
                    hucre->setStyleSheet(baseStyle + "color: #FF5555;");
                }
                // (Eğer hücre boşsa veya doğruysa, rengi zaten mavi olduğu için dokunmuyoruz.)
            }
        }
    }
    if (tumuDogru) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Sonuç");
        msgBox.setText("Tebrikler! Sudoku'yu doğru çözdünüz.");
        QPushButton *yeniSudoku = msgBox.addButton("Yeni Sudoku", QMessageBox::YesRole);
        QPushButton *hayirBtn = msgBox.addButton("Tamam",QMessageBox::NoRole);
        msgBox.exec();
        if (msgBox.clickedButton()==yeniSudoku){
            quint8 x;
            if(ui->zorlukcb->currentText()=="Kolay"){
                x=1;
            }else if (ui->zorlukcb->currentText()=="Orta"){
                x=2;
            }else if (ui->zorlukcb->currentText()=="Zor"){
                x=3;
            }
            MainWindow::yeniSudoku(x);
        }
    } else {
        QMessageBox::warning(this, "Sonuç", "Hatalar var veya bulmaca henüz bitmedi.");
    }
}

