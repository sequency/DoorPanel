#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QScrollBar>
#include <QListWidgetItem>
#include <QMovie>
#include <QTimer>
#include <QMediaPlaylist>
//#include <QtMultimedia>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
        void showEvent(QShowEvent *event);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QMediaPlayer *player1;
    QMediaPlayer *redAlert;
    QMediaPlaylist *redAlertPlaylist;
    int currentSliderValueZE;
    int currentSliderValueER;
    int MainVolume;
    int menuVolume;
    int alarmVolume;
    QUrl audio_processing;
    QUrl audio_beep;
    QUrl audio_denied;
    QUrl audio_canceled;
    QUrl audio_redalert;
    QMovie *redAlertAnim;
    QTimer *taskCheck;
    QString pathXMLZeiten;
    QString pathXMLTasks;
    QString pathXMLSettings;
    void deleteXMLElementZEITEN(QString ITEM_ID);
    void changeXMLElementZEITEN(QString ITEM_ID, QString TAG = "0", QString MONAT = "0", QString JAHR = "2017", QString BEGINN_H = "0", QString BEGINN_M = "0", QString ENDE_H = "0", QString ENDE_M = "0");
    bool loadingSettings = true;


private:
    Ui::MainWindow *ui;

public slots:
    void startAnimation();
    void taskIterator();
    void taskFunction();
    void zeitFunction();

private slots:

    void delay(int millisecondsToWait);
    void showTime();
    void window_shown();

    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_4_clicked();
    void on_spin_ZE_dd_plus_clicked();
    void on_spin_ZE_dd_minus_clicked();
    void on_spin_ZE_MM_plus_clicked();
    void on_spin_ZE_mm_minus_clicked();
    void on_spin_ZE_yy_plus_clicked();
    void on_spin_ZE_yy_minus_clicked();
    void on_spin_ZE_h_beginn_plus_clicked();
    void on_spin_ZE_h_beginn_ende_clicked();
    void on_spin_ZE_m_beginn_plus_clicked();
    void on_spin_ZE_m_beginn_minus_clicked();
    void on_spin_ZE_h_ende_plus_clicked();
    void on_spin_ZE_h_ende_minus_clicked();
    void on_spin_ZE_m_ende_plus_clicked();
    void on_spin_ZE_m_ende_minus_clicked();
    void on_sliderMusikPlaylist_2_valueChanged(int value);
    void on_slider_Zeiterfassung_Liste_valueChanged(int value);
    void on_slider_Erinnerungen_Liste_valueChanged(int value);
    void on_button_SYSTEM_clicked();
    void on_button_DEAKTIVIERT_2_clicked();
    void on_button_DEAKTIVIERT_1_clicked();
    void on_pushButton_20_clicked();
    void on_pushButton_21_clicked();
    void on_pushButton_22_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_23_clicked();
    void on_Erinnerungen_Liste_itemPressed(QListWidgetItem *item);

    void on_slider_Erinnerungen_Liste_sliderReleased();
    void on_Erinnerungen_Liste_itemClicked(QListWidgetItem *item);
    void on_Zeiterfassung_LISTE_itemClicked(QListWidgetItem *item);
    void on_pushButton_6_clicked();
    void getXMLitem(QString type, QString itemID, QString action = "r");
    void on_spin_ZE_dd_plus_neu_clicked();
    void on_spin_ZE_dd_minus_neu_clicked();
    void on_spin_ZE_MM_plus_neu_clicked();
    void on_spin_ZE_mm_minus_neu_clicked();
    void on_spin_ZE_yy_plus_neu_clicked();
    void on_spin_ZE_yy_minus_neu_clicked();
    void on_spin_ZE_h_beginn_plus_neu_clicked();
    void on_spin_ZE_h_beginn_ende_neu_clicked();
    void on_spin_ZE_m_beginn_plus_neu_clicked();
    void on_spin_ZE_m_beginn_minus_neu_clicked();
    void on_spin_ZE_h_ende_plus_neu_clicked();
    void on_spin_ZE_h_ende_minus_neu_clicked();
    void on_spin_ZE_m_ende_plus_neu_clicked();
    void on_spin_ZE_m_ende_minus_neu_clicked();
    void on_pushButton_15_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_14_clicked();
    void on_slider_Zeiterfassung_Liste_sliderReleased();

    void loadXmlZE();
    void loadXmlER();
    void on_pushButton_17_clicked();
    void on_pushButton_16_clicked();
    void on_redAlertText_clicked();
    void on_pushButton_19_clicked();
    void writeXML();
    void on_pushButton_8_clicked();
    void on_pushButton_38_clicked();
    void on_pushButton_9_clicked();
    void on_verticalSlider_5_valueChanged(int value);
    void on_verticalSlider_4_valueChanged(int value);
    void on_verticalSlider_3_valueChanged(int value);
    void on_pushButton_27_clicked();
    void keyPressEvent(QKeyEvent *event);
    void loadSettings(QString category);
    void saveXMLSettings();

    void on_pushButton_48_clicked();
    void on_verticalSlider_3_sliderReleased();
    void on_verticalSlider_4_sliderReleased();
    void on_verticalSlider_5_sliderReleased();
};

#endif // MAINWINDOW_H
