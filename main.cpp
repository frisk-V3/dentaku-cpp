#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGridLayout>
#include <windows.h>
#include <dwmapi.h>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QWidget window;
    window.setWindowTitle("Sharp Calc");
    QVBoxLayout *layout = new QVBoxLayout(&window);

    // 表示画面
    QLineEdit *display = new QLineEdit("0");
    display->setAlignment(Qt::AlignRight);
    display->setReadOnly(true);
    display->setStyleSheet("font-size: 24px; height: 50px;");
    layout->addWidget(display);

    QGridLayout *gridLayout = new QGridLayout();
    const char* labels[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"C", "0", "=", "+"}
    };

    // 計算用の変数（シンプルに実装）
    static double pendingValue = 0;
    static QString pendingOperator = "";
    static bool waitingForOperand = true;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            QString text = labels[i][j];
            QPushButton *btn = new QPushButton(text);
            btn->setMinimumHeight(60);
            gridLayout->addWidget(btn, i, j);

            // ボタンが押された時の処理（ラムダ式で接続）
            QObject::connect(btn, &QPushButton::clicked, [=]() {
                if (text >= "0" && text <= "9") {
                    if (waitingForOperand) {
                        display->setText(text);
                        waitingForOperand = false;
                    } else {
                        display->setText(display->text() + text);
                    }
                } else if (text == "C") {
                    display->setText("0");
                    pendingValue = 0;
                    waitingForOperand = true;
                } else if (text == "=") {
                    double val = display->text().toDouble();
                    if (pendingOperator == "+") pendingValue += val;
                    else if (pendingOperator == "-") pendingValue -= val;
                    else if (pendingOperator == "*") pendingValue *= val;
                    else if (pendingOperator == "/") pendingValue /= val;
                    display->setText(QString::number(pendingValue));
                    waitingForOperand = true;
                } else { // 演算子 (+, -, *, /)
                    pendingValue = display->text().toDouble();
                    pendingOperator = text;
                    waitingForOperand = true;
                }
            });
        }
    }
    layout->addLayout(gridLayout);
    window.show();

    // 角丸無効化
    HWND hwnd = (HWND)window.winId();
    DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    return a.exec();
}
