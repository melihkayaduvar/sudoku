#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    srand(time(nullptr)); // Rastgele sayı üretimi için sistem saatini başlangıç (seed) olarak kullanır

    // --- OTOMATİK HÜCRE YAPILANDIRMA ---
    // 9x9'luk Sudoku tablosundaki her bir QLineEdit hücresine döngüyle erişiyoruz.
    for(int i = 0; i < 9; ++i){
        for(int j = 0; j < 9; ++j){
            // Tasarım ekranında m_0x0, m_0x1... şeklinde isimlendirdiğimiz kutucukları bulur
            QString widgetAdi = QString("m_%1x%2").arg(i).arg(j);
            QLineEdit *hucre = this->findChild<QLineEdit *>(widgetAdi);

            if(hucre){
                hucre->setMaxLength(1); // Kutucuğa en fazla 1 karakter girilmesini sağlar
                // Sadece 1 ile 9 arasındaki tam sayıların girilmesine izin verir
                QValidator *validator = new QIntValidator(1, 9, this);
                hucre->setValidator(validator);

                // Hücredeki metin her değiştiğinde 'on_hucreDegisti' fonksiyonunu tetikler (Sinyal-Slot)
                connect(hucre, &QLineEdit::textChanged, this, &MainWindow::on_hucreDegisti);
            }
        }
    }

    // --- GÖRSEL TASARIM (QSS - Qt Style Sheets) ---
    // Zorluk seçim kutusunu (ComboBox) koyu tema ile özelleştirir
    ui->zorlukcb->setStyleSheet(
        "QComboBox { background-color: #2E2E2E; color: white; border: 1px solid #555555; border-radius: 5px; padding-left: 10px; }"
        "QComboBox QAbstractItemView { background-color: #2E2E2E; color: white; selection-background-color: #6A0DAD; selection-color: white; }"
        "QComboBox::drop-down { border: none; }"
        );

    yeniSudoku(1); // Program ilk açıldığında 'Kolay' modda bir oyun başlatır
}

MainWindow::~MainWindow(){
    delete ui;
}

// --- KURAL DENETLEYİCİ ---
// Bir sayının belirtilen hücreye yerleştirilmesinin Sudoku kurallarına uygun olup olmadığını kontrol eder.
bool MainWindow::uygunmu(quint8 board[9][9], int row, int col, int num) {
    // Satır ve Sütun Kontrolü: Aynı rakam o satırda veya sütunda var mı?
    for (int x = 0; x < 9; x++) if (board[row][x] == num) return false;
    for (int x = 0; x < 9; x++) if (board[x][col] == num) return false;

    // 3x3 Blok Kontrolü: Hücrenin bulunduğu küçük kare içinde aynı rakam var mı?
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i + startRow][j + startCol] == num) return false;
        }
    }
    return true; // Eğer hiçbir kural ihlal edilmediyse sayı yerleştirilebilir.
}

// --- BULMACA OLUŞTURUCU (Backtracking) ---
// Boş matrisi kurallara uygun şekilde tamamen dolduran algoritma.
bool MainWindow::matrisiDoldur(quint8 board[9][9]) {
    int row, col;
    bool isEmpty = false;
    // Matriste boş (değeri 0 olan) bir hücre arar
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            if (board[row][col] == 0) { isEmpty = true; break; }
        }
        if (isEmpty) break;
    }
    if (!isEmpty) return true; // Boş yer kalmadıysa matris başarıyla doldurulmuştur.
    // Sayıları (1-9) her seferinde farklı bir sırayla denemek için listeyi karıştırır
    int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for(int i=0; i<9; ++i) {
        int r = rand() % 9;
        std::swap(nums[i], nums[r]);
    }
    // Deneme-Yanılma süreci
    for (int i = 0; i < 9; i++) {
        int num = nums[i];
        if (uygunmu(board, row, col, num)) { // Eğer sayı kurallara uygunsa
            board[row][col] = num;           // Sayıyı yerleştir
            if (matrisiDoldur(board)) return true; // Bir sonraki hücreye geç (recursive)
            board[row][col] = 0;             // Hatalı yola girildiyse geri al
        }
    }
    return false;
}

// --- YENİ OYUN BAŞLATMA ---
void MainWindow::yeniSudoku(int zorluk)
{
    // Önce tamamen doğru çözülmüş bir matris üretir
    memset(cozumMatrisi, 0, sizeof(cozumMatrisi));// çözüm matrisini 0 matrisine çevirir
    matrisiDoldur(cozumMatrisi);
    // Çözülmüş hali, üzerinde oynanacak olan 'sudokuMatrisi'ne kopyalar
    memcpy(sudokuMatrisi, cozumMatrisi, sizeof(cozumMatrisi));

    // Zorluk seviyesine göre silinecek hücre sayısını belirler
    int hucresil = 40;
    if (zorluk == 2) hucresil = 45;
    else if (zorluk == 3) hucresil = 50;
    else if (zorluk == 4) hucresil = 5; // Test/Hızlı mod

    // Rastgele hücreleri temizleyerek kullanıcıya boşluklar bırakır
    int silinenHucresayisi = 0;
    while (silinenHucresayisi < hucresil) {
        int satir = rand() % 9;
        int sutun = rand() % 9;
        if (sudokuMatrisi[satir][sutun] != 0) {
            sudokuMatrisi[satir][sutun] = 0;
            silinenHucresayisi++;
        }
    }

    // Arayüzü günceller: Sabit sayıları kilitler, boşlukları mavi yapar
    QString baseStyle = "background-color: #2E2E2E; border: 1px solid #555555; border-radius: 5px; text-align: center; font-size: 16pt; font-weight: bold;";
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            QLineEdit *hucre = this->findChild<QLineEdit *>(QString("m_%1x%2").arg(i).arg(j));
            if (hucre) {
                int sayi = sudokuMatrisi[i][j];
                if (sayi != 0) {
                    hucre->setText(QString::number(sayi));
                    hucre->setReadOnly(true); // Önceden verilen sayılar değiştirilemez
                    hucre->setStyleSheet(baseStyle + "color: white;");
                } else {
                    hucre->clear(); // Kullanıcının dolduracağı kısımlar
                    hucre->setReadOnly(false);
                    hucre->setStyleSheet(baseStyle + "color: #55E0FF;");
                }
            }
        }
    }
}

// --- ANLIK HATA KONTROLÜ ---
// Kullanıcı bir hücreye sayı yazdığında çalışan fonksiyon.
void MainWindow::on_hucreDegisti()
{
    QLineEdit *hucre = qobject_cast<QLineEdit*>(sender());
    if (!hucre) return;

    // Değişen hücrenin adından koordinatlarını (satir, sutun) çıkarır
    QString ad = hucre->objectName().remove(0, 2);
    QStringList parts = ad.split('x');
    int satir = parts[0].toInt();
    int sutun = parts[1].toInt();
    QString text = hucre->text();

    if (text == "0") { hucre->clear(); return; }

    QString baseStyle = "background-color: #2E2E2E; border: 1px solid #555555; border-radius: 5px; text-align: center; font-size: 16pt; font-weight: bold;";

    if (text.isEmpty()) {
        sudokuMatrisi[satir][sutun] = 0;
        hucre->setStyleSheet(baseStyle + "color: #55E0FF;");
        return;
    }

    int sayi = text.toInt();

    // Sayıyı geçici olarak matristen çıkarıp 'uygunmu' testine sokar
    sudokuMatrisi[satir][sutun] = 0;
    bool guvenliMi = uygunmu(sudokuMatrisi, satir, sutun, sayi);
    sudokuMatrisi[satir][sutun] = sayi;

    // Eğer girilen sayı kuralları bozuyorsa hücre KIRMIZI olur, bozmuyorsa MAVİ kalır
    if (guvenliMi) {
        hucre->setStyleSheet(baseStyle + "color: #55E0FF;");
    } else {
        hucre->setStyleSheet(baseStyle + "color: #FF5555;");
    }
}

// --- BUTON: YENİ OYUN ---
void MainWindow::on_yenisudoku_clicked()
{
    quint8 x = 1; // Varsayılan: Kolay
    QString secim = ui->zorlukcb->currentText();
    if (secim == "Orta") x = 2;
    else if (secim == "Zor") x = 3;
    else if (secim == "Test") x = 4;

    yeniSudoku(x); // Seçilen zorlukta yeni tablo oluşturur
}

// --- BUTON: KONTROL ET ---
// Oyuncunun girdiği sayıları doğru çözümle karşılaştırır.
void MainWindow::on_kontrolet_clicked()
{
    QString baseStyle = "background-color: #2E2E2E; border: 1px solid #555555; border-radius: 5px; text-align: center; font-size: 16pt; font-weight: bold;";

    bool tumuDogru = true;
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            QLineEdit *hucre = this->findChild<QLineEdit *>(QString("m_%1x%2").arg(i).arg(j));
            if (!hucre) continue;

            // Sadece kullanıcının doldurduğu (kilitli olmayan) hücreleri kontrol eder
            if (!hucre->isReadOnly()) {
                if (sudokuMatrisi[i][j] != cozumMatrisi[i][j]) {
                    tumuDogru = false;
                    hucre->setStyleSheet(baseStyle + "color: #FF5555;"); // Yanlış hücreyi kırmızı yapar
                }
            }
        }
    }

    if (tumuDogru) {
        // Hepsi doğruysa tebrik mesajı ve yeni oyun teklifi sunar
        QMessageBox msgBox;
        msgBox.setWindowTitle("Sonuç");
        msgBox.setText("Tebrikler! Sudoku'yu doğru çözdünüz.");
        QPushButton *yeniBtn = msgBox.addButton("Yeni Sudoku", QMessageBox::YesRole);
        msgBox.addButton("Tamam", QMessageBox::NoRole);
        msgBox.exec();

        if (msgBox.clickedButton() == yeniBtn) on_yenisudoku_clicked();
    } else {
        QMessageBox::warning(this, "Sonuç", "Hatalar Var veya Bulmaca Henüz Bitmedi.");
    }
}
