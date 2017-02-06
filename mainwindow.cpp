#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFontDatabase>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QSqlDatabase>
#include <QKeyEvent>
#include <QSqlQuery>
#include <QSqlResult>
#include <QMediaPlayer>
#include <QListWidget>
#include <QScrollBar>
#include <QtXml>
#include <QFile>
#include <QTextStream>
#include <stdio.h>
#include <QDomElement>
#include <QMovie>
#include <QTimer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QUrl>


//#include <QtMultimedia>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
      ui->setupUi(this);

      /**********************###### DATEN ######**********************/
  pathXMLSettings="/mnt/sdcard/DoorPanel/settings.xml";
  //pathXMLZeiten="/mnt/sdcard/DoorPanel/zeiten.xml";
  //pathXMLTasks="/mnt/sdcard/DoorPanel/tasks.xml";
  loadSettings("AUDIO");
  loadSettings("PFADE");



  ui->Zeiterfassung_LISTE->clear();
  ui->Erinnerungen_Liste->clear();
  loadXmlER();
  loadXmlZE();
  ui->slider_Zeiterfassung_Liste->setMaximum(ui->Zeiterfassung_LISTE->count() - 5);
  ui->slider_Erinnerungen_Liste->setMaximum(ui->Erinnerungen_Liste->count() - 5);

  loadingSettings = false;
  /**********************###### FONTS ######**********************/

  /**********************###### FONTS ######**********************/
  int id = QFontDatabase::addApplicationFont(
      ":/fonts/Helvetica_Ultra_Compressed.ttf");
  id += QFontDatabase::addApplicationFont(":/fonts/webdings.ttf");
  QString family = QFontDatabase::applicationFontFamilies(id).at(0);
  QFont huc(family);

  QFile res(":/fonts/Helvetica_Ultra_Compressed.ttf");
  if (!res.open(QIODevice::ReadOnly)) {
    qDebug() << "nixda";
  }

  QFile res2(":/fonts/webdings.ttf");
  if (!res2.open(QIODevice::ReadOnly)) {
    qDebug() << "nixda";
  }

  QFont f2;
  f2.setFamily("Webdings");
  f2.setPointSize(12);

  QFont f;
  f.setFamily("Helvetica Ultra Compressed");
  f.setPointSize(12);
  ui->label->setFont(huc);
  /**********************###### ----- ######**********************/



  /**********************###### AUDIO ######**********************/
  audio_processing = QUrl("qrc:/sounds/processing.mp3");
  audio_beep = QUrl("qrc:/sounds/computerbeep_15.mp3");
  audio_denied = QUrl("qrc:/sounds/denybeep3.mp3");
  audio_canceled = QUrl("qrc:/sounds/input_failed2_clean.mp3");
  audio_redalert = QUrl("qrc:/sounds/tng_red_alert1.mp3");
  player1 = new QMediaPlayer;
  redAlert = new QMediaPlayer;

  //MainVolume = 0;
  //menuVolume = (100 * MainVolume) / 100;
  //alarmVolume = (100 * MainVolume) / 100;
  redAlert->setVolume(alarmVolume);
  player1->setVolume(menuVolume);

  redAlertPlaylist = new QMediaPlaylist;
  redAlertPlaylist->addMedia(audio_redalert);
  redAlertPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
  //playlist->setCurrentIndex(1);
  redAlert->setPlaylist(redAlertPlaylist);

  /**********************###### ----- ######**********************/



  /**********************###### STYLE ######**********************/
  QList<QWidget *> qwidgets = this->findChildren<QWidget *>();
  for (int i = 0; i < qwidgets.count(); i++) {
    // qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_standard").toString());
    qwidgets[i]->setProperty("css_standard",qwidgets[i]->styleSheet());
    qwidgets[i]->setFont(huc);
  }
  /**********************###### ----- ######**********************/



  /**********************###### DATUM ######**********************/
  QString s = QDate::currentDate().toString("ddd dd. MM. yyyy").toUpper();
  ui->label->setText(s);

  QTime time1 = QTime::currentTime();

  ui->spin_ZE_dd->setValue(QDate::currentDate().toString("dd").toInt());
  ui->spin_ZE_MM->setValue(QDate::currentDate().toString("MM").toInt());
  ui->spin_ZE_yy->setValue(QDate::currentDate().toString("yyy").toInt());

  ui->spin_ZE_h_beginn->setValue(time1.toString("hh").toInt());
  ui->spin_ZE_m_beginn->setValue(time1.toString("mm").toInt());

  ui->spin_ZE_h_ende->setValue(time1.toString("hh").toInt());
  ui->spin_ZE_m_ende->setValue(time1.toString("mm").toInt());
  /**********************###### ----- ######**********************/



  /**********************###### TIMER ######**********************/
  /*QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
  timer->start(1000);
  showTime();

  taskCheck = new QTimer(this);
  connect(taskCheck, SIGNAL(timeout()), this, SLOT(taskIterator()));
  timer->start(5000);*/

  taskFunction();
  zeitFunction();

  ui->INHALT->setCurrentWidget(ui->TAB_KALENDER);
  ui->MENU->setCurrentWidget(ui->menu_1);

  ui->calendarWidget->setSelectedDate(QDate::currentDate());
  redAlertAnim = new QMovie(":/images/redalert.gif");

  /**********************###### ----- ######**********************/




}



void MainWindow::showEvent(QShowEvent *event) {

  QMainWindow::showEvent(event);
  QTimer::singleShot(50, this, SLOT(window_shown()));

  return;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Back)
        qDebug() << "[[Back button]]";
    else if (event->key() == Qt::Key_Menu)
        qDebug() << "[[Menu button]]";
    else if (event->key() == Qt::Key_TopMenu)
        qDebug() << "[[Top menu button]]";
    else
        qDebug() << "[[some other button]]";

    event->accept();
}


void MainWindow::taskFunction() {
    /*taskCheck = new QTimer(this);
    connect(taskCheck, SIGNAL(timeout()), this, SLOT(taskIterator()));
    taskCheck->start(60000);
    taskIterator();*/
}

void MainWindow::zeitFunction() {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);
    showTime();
}

void MainWindow::window_shown() {

  // player->setMedia(QUrl::fromLocalFile(mediaDir + "processing.mp3"));
  // qDebug() << mediaDir+"processing.mp3";
  // player->play();

  player1->setVolume(menuVolume);
  player1->setMedia(audio_processing);
  player1->play();

  // startAnimation();
  return;
}

void MainWindow::delay(int millisecondsToWait) {
  QTime dieTime = QTime::currentTime().addMSecs(millisecondsToWait);
  while (QTime::currentTime() < dieTime) {
    QCoreApplication::processEvents(QEventLoop::AllEvents);
  }
}

void MainWindow::taskIterator() {



    QTime time1 = QTime::currentTime();

    QString CHECK_TAG_NAME = QDate::currentDate().toString("ddd").left(2).toUpper();
    QString CHECK_TAG = QDate::currentDate().toString("dd");
    QString CHECK_MONAT = QDate::currentDate().toString("MM");
    QString CHECK_JAHR = QDate::currentDate().toString("yyyy");
    QString CHECK_STUNDE = time1.toString("hh");
    QString CHECK_MINUTE = time1.toString("mm");

    //QString s = QDate::currentDate().toString("ddd dd. MM. yyyy").toUpper();



    // The QDomDocument class represents an XML document.
    QDomDocument xmlBOM;
    // Load xml file as raw data
    QFile fxml(pathXMLTasks);
    if (!fxml.open(QIODevice::ReadOnly)) {
      // Error while loading file
      qDebug() << "Error while loading file";
      // return 1;
    }
    // Set data into the QDomDocument before processing
    xmlBOM.setContent(&fxml);
    fxml.close();

    // Extract the root markup
    QDomElement root = xmlBOM.documentElement();

    // Get root names and attributes
    QString Type = root.tagName();
    QString DEVICE = root.attribute("DEVICE", "No name");
    QString CATEGORY = root.attribute("CATEGORY", "1900");

    // Get the first child of the root (Markup COMPONENT is expected)
    QDomElement Component = root.firstChild().toElement();

    // Loop while there is a child
    while (!Component.isNull()) {
      // Check if the child tag name is COMPONENT
      if (Component.tagName() == "ENTRY") {

        // Read and display the component ID
        QString ID = Component.attribute("ID", "No ID");

        // Get the first child of the component
        QDomElement Child = Component.firstChild().toElement();

        QString WIEDERHOLUNG;
        QString TEXTKURZ;
        QString TEXTLANG;
        QString TAG;
        QString TAG_NAME;
        QString MONAT;
        QString JAHR;
        QString STUNDE;
        QString MINUTE;

        // Read each child of the component node
        while (!Child.isNull()) {
          // Read Name and value
            if (Child.tagName() == "WIEDERHOLUNG") {
                WIEDERHOLUNG=Child.firstChild().toText().data();
            }
            if (Child.tagName() == "TAG") {
                TAG=Child.firstChild().toText().data();
            }
            if (Child.tagName() == "TAGNAME") {
                TAG_NAME=Child.firstChild().toText().data();
            }
            if (Child.tagName() == "MONAT") {
                MONAT=Child.firstChild().toText().data();
            }
            if (Child.tagName() == "JAHR") {
                JAHR=Child.firstChild().toText().data();
            }
            if (Child.tagName() == "STUNDE") {
                STUNDE=Child.firstChild().toText().data();
            }
            if (Child.tagName() == "MINUTE") {
                MINUTE=Child.firstChild().toText().data();
            }
            if (Child.tagName() == "TEXTKURZ") {
                TEXTKURZ=Child.firstChild().toText().data();
            }
            if (WIEDERHOLUNG == "D") {

                    if (CHECK_STUNDE == STUNDE && CHECK_MINUTE == MINUTE) {

                        ui->INHALT->setCurrentWidget(ui->TAB_ERINNERUNGEN);
                        ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_ALARM);
                        getXMLitem("ERINNERUNGEN",ID,"r");
                        ui->redAlertText->setText(TEXTKURZ);
                        ui->redAlertLabel->setMovie(redAlertAnim);
                        redAlertAnim->start();
                        QList<QWidget *> qwidgets = this->findChildren<QWidget *>();
                        for (int i = 0; i < qwidgets.count(); i++) {
                          // qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_standard").toString());
                          qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_red").toString());
                        }

                        redAlert->play();
                    }

            } else

            if (WIEDERHOLUNG=="W") {

                if (CHECK_TAG_NAME == TAG) {

                    if (CHECK_STUNDE == STUNDE && CHECK_MINUTE == MINUTE) {

                        ui->INHALT->setCurrentWidget(ui->TAB_ERINNERUNGEN);
                        ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_ALARM);
                        getXMLitem("ERINNERUNGEN",ID,"r");
                        ui->redAlertText->setText(TEXTKURZ);
                        ui->redAlertLabel->setMovie(redAlertAnim);
                        redAlertAnim->start();
                        QList<QWidget *> qwidgets = this->findChildren<QWidget *>();
                        for (int i = 0; i < qwidgets.count(); i++) {
                          // qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_standard").toString());
                          qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_red").toString());
                        }

                        redAlert->play();
                    }
                }
            } else

            if (WIEDERHOLUNG=="E") {

                if (CHECK_TAG == TAG && CHECK_MONAT == MONAT && CHECK_JAHR == JAHR && CHECK_STUNDE == STUNDE && CHECK_MINUTE == MINUTE) {
                        ui->INHALT->setCurrentWidget(ui->TAB_ERINNERUNGEN);
                        ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_ALARM);
                        getXMLitem("ERINNERUNGEN",ID,"r");
                        ui->redAlertText->setText(TEXTKURZ);
                        ui->redAlertLabel->setMovie(redAlertAnim);
                        redAlertAnim->start();
                        QList<QWidget *> qwidgets = this->findChildren<QWidget *>();
                        for (int i = 0; i < qwidgets.count(); i++) {
                          // qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_standard").toString());
                          qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_red").toString());
                        }
                        redAlert->play();

                }
            } else

            if (WIEDERHOLUNG=="J") {

                if (CHECK_TAG == TAG && CHECK_MONAT == MONAT && CHECK_STUNDE == STUNDE && CHECK_MINUTE == MINUTE) {
                        ui->INHALT->setCurrentWidget(ui->TAB_ERINNERUNGEN);
                        ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_ALARM);
                        getXMLitem("ERINNERUNGEN",ID,"r");
                        ui->redAlertText->setText(TEXTKURZ);
                        ui->redAlertLabel->setMovie(redAlertAnim);
                        redAlertAnim->start();
                        QList<QWidget *> qwidgets = this->findChildren<QWidget *>();
                        for (int i = 0; i < qwidgets.count(); i++) {
                          // qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_standard").toString());
                          qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_red").toString());
                        }
                        redAlert->play();

                }
            } else

            if (WIEDERHOLUNG=="M") {

                if (CHECK_TAG == TAG && CHECK_STUNDE == STUNDE && CHECK_MINUTE == MINUTE) {
                        ui->INHALT->setCurrentWidget(ui->TAB_ERINNERUNGEN);
                        ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_ALARM);
                        getXMLitem("ERINNERUNGEN",ID,"r");
                        ui->redAlertText->setText(TEXTKURZ);
                        ui->redAlertLabel->setMovie(redAlertAnim);
                        redAlertAnim->start();
                        QList<QWidget *> qwidgets = this->findChildren<QWidget *>();
                        for (int i = 0; i < qwidgets.count(); i++) {
                          // qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_standard").toString());
                          qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_red").toString());
                        }
                        redAlert->play();

                }
            }



          // Next child
          Child = Child.nextSibling().toElement();
        }

        QListWidgetItem *eintrag = new QListWidgetItem;
        QString searchEntry = ID;
        QVariant elementDaten(searchEntry);

        eintrag->setData(Qt::UserRole, elementDaten);
        eintrag->setText(TEXTKURZ);

        ui->Erinnerungen_Liste->addItem(eintrag);
      }

      // Next component

      Component = Component.nextSibling().toElement();
    }






    }

void MainWindow::startAnimation() {

  /*******************************************************************/

  /*QGraphicsOpacityEffect *pushButton_7_effect =
      new QGraphicsOpacityEffect(ui->pushButton_7);
  pushButton_7_effect->setOpacity(0.5);
  ui->pushButton_7->setGraphicsEffect(pushButton_7_effect);
  QGraphicsOpacityEffect *pushButton_6_effect =
      new QGraphicsOpacityEffect(ui->pushButton_6);
  pushButton_6_effect->setOpacity(0.5);
  ui->pushButton_6->setGraphicsEffect(pushButton_6_effect);
  QGraphicsOpacityEffect *pushButton_5_effect =
      new QGraphicsOpacityEffect(ui->pushButton_5);
  pushButton_5_effect->setOpacity(0.5);
  ui->pushButton_5->setGraphicsEffect(pushButton_5_effect);
  QGraphicsOpacityEffect *pushButton_4_effect =
      new QGraphicsOpacityEffect(ui->pushButton_4);
  pushButton_4_effect->setOpacity(0.5);
  ui->pushButton_4->setGraphicsEffect(pushButton_4_effect);
  QGraphicsOpacityEffect *pushButton_3_effect =
      new QGraphicsOpacityEffect(ui->pushButton_3);
  pushButton_3_effect->setOpacity(0.5);
  ui->pushButton_3->setGraphicsEffect(pushButton_3_effect);
  QGraphicsOpacityEffect *Rundung1_2_effect =
      new QGraphicsOpacityEffect(ui->Rundung1_2);
  Rundung1_2_effect->setOpacity(0.5);
  ui->Rundung1_2->setGraphicsEffect(Rundung1_2_effect);
  QGraphicsOpacityEffect *linie1_9_effect =
      new QGraphicsOpacityEffect(ui->linie1_9);
  linie1_9_effect->setOpacity(0.5);
  ui->linie1_9->setGraphicsEffect(linie1_9_effect);
  QGraphicsOpacityEffect *linie3_9_effect =
      new QGraphicsOpacityEffect(ui->linie3_9);
  linie3_9_effect->setOpacity(0.5);
  ui->linie3_9->setGraphicsEffect(linie3_9_effect);
  QGraphicsOpacityEffect *linie5_21_effect =
      new QGraphicsOpacityEffect(ui->linie5_21);
  linie5_21_effect->setOpacity(0.5);
  ui->linie5_21->setGraphicsEffect(linie5_21_effect);
  QGraphicsOpacityEffect *linie3_11_effect =
      new QGraphicsOpacityEffect(ui->linie3_11);
  linie3_11_effect->setOpacity(0.5);
  ui->linie3_11->setGraphicsEffect(linie3_11_effect);
  QGraphicsOpacityEffect *linie_links0_2_effect =
      new QGraphicsOpacityEffect(ui->linie_links0_2);
  linie_links0_2_effect->setOpacity(0.5);
  ui->linie_links0_2->setGraphicsEffect(linie_links0_2_effect);
  QGraphicsOpacityEffect *menu_rund_ol_effect =
      new QGraphicsOpacityEffect(ui->menu_rund_ol);
  menu_rund_ol_effect->setOpacity(0.5);
  ui->menu_rund_ol->setGraphicsEffect(menu_rund_ol_effect);
  QGraphicsOpacityEffect *linie0_effect =
      new QGraphicsOpacityEffect(ui->linie0);
  linie0_effect->setOpacity(0.5);
  ui->linie0->setGraphicsEffect(linie0_effect);
  QGraphicsOpacityEffect *lblh_effect = new QGraphicsOpacityEffect(ui->lblh);
  lblh_effect->setOpacity(0.5);
  ui->lblh->setGraphicsEffect(lblh_effect);
  QGraphicsOpacityEffect *lbldoppelpunkt1_effect =
      new QGraphicsOpacityEffect(ui->lbldoppelpunkt1);
  lbldoppelpunkt1_effect->setOpacity(0.5);
  ui->lbldoppelpunkt1->setGraphicsEffect(lbldoppelpunkt1_effect);
  QGraphicsOpacityEffect *lblm_effect = new QGraphicsOpacityEffect(ui->lblm);
  lblm_effect->setOpacity(0.5);
  ui->lblm->setGraphicsEffect(lblm_effect);
  QGraphicsOpacityEffect *lbldoppelpunkt2_effect =
      new QGraphicsOpacityEffect(ui->lbldoppelpunkt2);
  lbldoppelpunkt2_effect->setOpacity(0.5);
  ui->lbldoppelpunkt2->setGraphicsEffect(lbldoppelpunkt2_effect);
  QGraphicsOpacityEffect *lbls_effect = new QGraphicsOpacityEffect(ui->lbls);
  lbls_effect->setOpacity(0.5);
  ui->lbls->setGraphicsEffect(lbls_effect);
  QGraphicsOpacityEffect *linie0_2_effect =
      new QGraphicsOpacityEffect(ui->linie0_2);
  linie0_2_effect->setOpacity(0.5);
  ui->linie0_2->setGraphicsEffect(linie0_2_effect);
  QGraphicsOpacityEffect *linie7_effect =
      new QGraphicsOpacityEffect(ui->linie7);
  linie7_effect->setOpacity(0.5);
  ui->linie7->setGraphicsEffect(linie7_effect);*/

  /*******************************************************************/
  /*ui->pushButton_7->setVisible(true);
  delay(60);
  pushButton_7_effect->setOpacity(0.75);
  ui->pushButton_7->setGraphicsEffect(pushButton_7_effect);
  delay(60);
  pushButton_7_effect->setOpacity(1.0);
  ui->pushButton_7->setGraphicsEffect(pushButton_7_effect);
  delay(60);

  ui->pushButton_6->setVisible(true);
  pushButton_6_effect->setOpacity(0.75);
  ui->pushButton_6->setGraphicsEffect(pushButton_6_effect);
  delay(60);
  pushButton_6_effect->setOpacity(1.0);
  ui->pushButton_6->setGraphicsEffect(pushButton_6_effect);
  delay(60);

  ui->pushButton_5->setVisible(true);
  pushButton_5_effect->setOpacity(0.75);
  ui->pushButton_5->setGraphicsEffect(pushButton_5_effect);
  delay(60);
  pushButton_5_effect->setOpacity(1.0);
  ui->pushButton_5->setGraphicsEffect(pushButton_5_effect);
  delay(60);

  ui->pushButton_4->setVisible(true);
  pushButton_4_effect->setOpacity(0.75);
  ui->pushButton_4->setGraphicsEffect(pushButton_4_effect);
  delay(60);
  pushButton_4_effect->setOpacity(1.0);
  ui->pushButton_4->setGraphicsEffect(pushButton_4_effect);
  delay(60);

  ui->pushButton_3->setVisible(true);
  pushButton_3_effect->setOpacity(0.75);
  ui->pushButton_3->setGraphicsEffect(pushButton_3_effect);
  ui->linie_links0_2->setVisible(true);
  linie_links0_2_effect->setOpacity(0.75);
  ui->linie_links0_2->setGraphicsEffect(linie_links0_2_effect);
  delay(60);
  pushButton_3_effect->setOpacity(1.0);
  ui->pushButton_3->setGraphicsEffect(pushButton_3_effect);
  linie_links0_2_effect->setOpacity(1.0);
  ui->linie_links0_2->setGraphicsEffect(linie_links0_2_effect);
  delay((60));

  ui->Rundung1_2->setVisible(true);
  Rundung1_2_effect->setOpacity(0.75);
  ui->Rundung1_2->setGraphicsEffect(Rundung1_2_effect);
  ui->menu_rund_ol->setVisible(true);
  menu_rund_ol_effect->setOpacity(0.75);
  ui->menu_rund_ol->setGraphicsEffect(menu_rund_ol_effect);

  delay(60);
  Rundung1_2_effect->setOpacity(1.0);
  ui->Rundung1_2->setGraphicsEffect(Rundung1_2_effect);
  menu_rund_ol_effect->setOpacity(1.0);
  ui->menu_rund_ol->setGraphicsEffect(menu_rund_ol_effect);
  delay(60);

  ui->linie1_9->setVisible(true);
  linie1_9_effect->setOpacity(0.75);
  ui->linie1_9->setGraphicsEffect(linie1_9_effect);
  ui->linie0->setVisible(true);
  linie0_effect->setOpacity(0.75);
  ui->linie0->setGraphicsEffect(linie0_effect);
  delay((60));
  linie1_9_effect->setOpacity(1.0);
  ui->linie1_9->setGraphicsEffect(linie1_9_effect);
  linie0_effect->setOpacity(1.0);
  ui->linie0->setGraphicsEffect(linie0_effect);
  delay((60));

  ui->linie3_9->setVisible(true);
  linie3_9_effect->setOpacity(0.75);
  ui->linie3_9->setGraphicsEffect(linie3_9_effect);
  ui->lblh->setVisible(true);
  lblh_effect->setOpacity(0.75);
  ui->lblh->setGraphicsEffect(lblh_effect);
  ui->lbldoppelpunkt1->setVisible(true);
  lbldoppelpunkt1_effect->setOpacity(0.75);
  ui->lbldoppelpunkt1->setGraphicsEffect(lbldoppelpunkt1_effect);
  delay(60);
  linie3_9_effect->setOpacity(1.0);
  ui->linie3_9->setGraphicsEffect(linie3_9_effect);
  lblh_effect->setOpacity(1.0);
  ui->lblh->setGraphicsEffect(lblh_effect);
  lbldoppelpunkt1_effect->setOpacity(1.0);
  ui->lbldoppelpunkt1->setGraphicsEffect(lbldoppelpunkt1_effect);
  delay(30);

  ui->linie5_21->setVisible(true);
  linie5_21_effect->setOpacity(0.75);
  ui->linie5_21->setGraphicsEffect(linie5_21_effect);

  ui->lblm->setVisible(true);
  lblm_effect->setOpacity(0.75);
  ui->lblm->setGraphicsEffect(lblm_effect);
  ui->lbldoppelpunkt2->setVisible(true);
  lbldoppelpunkt2_effect->setOpacity(0.75);
  ui->lbldoppelpunkt2->setGraphicsEffect(lbldoppelpunkt2_effect);
  ui->lbls->setVisible(true);
  lbls_effect->setOpacity(0.75);
  ui->lbls->setGraphicsEffect(lbls_effect);
  ui->linie0_2->setVisible(true);
  linie0_2_effect->setOpacity(0.75);
  ui->linie0_2->setGraphicsEffect(linie0_2_effect);

  delay(60);
  linie5_21_effect->setOpacity(1.0);
  ui->linie5_21->setGraphicsEffect(linie5_21_effect);
  lblm_effect->setOpacity(1.0);
  ui->lblm->setGraphicsEffect(lblm_effect);

  lbldoppelpunkt2_effect->setOpacity(1.0);
  ui->lbldoppelpunkt2->setGraphicsEffect(lbldoppelpunkt2_effect);

  lbls_effect->setOpacity(1.0);
  ui->lbls->setGraphicsEffect(lbls_effect);

  linie0_2_effect->setOpacity(1.0);
  ui->linie0_2->setGraphicsEffect(linie0_2_effect);
  delay((60));

  ui->TITEL->setVisible(true);
  ui->TITEL->setStyleSheet(TitelSTD_Stylesheet + "\ncolor:#404040;");
  ui->linie3_11->setVisible(true);
  linie3_11_effect->setOpacity(0.75);
  ui->linie3_11->setGraphicsEffect(linie3_11_effect);

  ui->TITEL->setStyleSheet(TitelSTD_Stylesheet + "\ncolor:#808080;");
  delay((60));

  ui->TITEL->setStyleSheet(TitelSTD_Stylesheet + "\ncolor:#ffffff;");
  linie3_11_effect->setOpacity(1.0);
  ui->linie3_11->setGraphicsEffect(linie3_11_effect);
  delay(60);
  delay((60));

  ui->linie7->setVisible(true);
  linie7_effect->setOpacity(0.75);
  ui->linie7->setGraphicsEffect(linie7_effect);
  delay((60));
  linie7_effect->setOpacity(1.0);
  ui->linie7->setGraphicsEffect(linie7_effect);
  delay((60));*/
}

void MainWindow::showTime() {
  QTime time = QTime::currentTime();
  QString text = time.toString("hh:mm:ss");
  /*if ((time.second() % 2) == 0)
      text[2] = ' ';*/
  if (time.second() == 0) {
      taskIterator();
  }
  ui->labelZeit->setText(text);
  if (time.hour() == 0) {
  QString s = QDate::currentDate().toString("ddd dd. MM. yyyy").toUpper();
  ui->label->setText(s);
  }
}

void MainWindow::loadSettings(QString category) {


    // The QDomDocument class represents an XML document.
    QDomDocument xmlBOM;
    // Load xml file as raw data
    QFile fxml(pathXMLSettings);
    if (!fxml.open(QIODevice::ReadOnly)) {
      // Error while loading file
      qDebug() << "Error while loading file";
      // return 1;
    }
    // Set data into the QDomDocument before processing
    xmlBOM.setContent(&fxml);
    fxml.close();

    // Extract the root markup
    QDomElement root = xmlBOM.documentElement();

    // Get root names and attributes
    QString Type = root.tagName();
    QString DEVICE = root.attribute("DEVICE", "No name");
    QString CATEGORY = root.attribute("CATEGORY", "1900");

    // Get the first child of the root (Markup COMPONENT is expected)
    QDomElement Component = root.firstChild().toElement();

    // Loop while there is a child
    while (!Component.isNull()) {
      // Check if the child tag name is COMPONENT
      if (Component.tagName() == "ENTRY") {


          // Read and display the component ID
          QString ID = Component.attribute("ID", "No ID");

          // Get the first child of the component
          QDomElement Child = Component.firstChild().toElement();

        // Read each child of the component node
        while (!Child.isNull()) {
            if (ID == category && category == "AUDIO") {

            if (Child.tagName() == "MAINVOLUME") {
                QString tempStringVolume=Child.firstChild().toText().data();
                int tempIntVolume=tempStringVolume.toInt();
                MainVolume = tempIntVolume;
                ui->verticalSlider_5->setValue(MainVolume);
            }
            if (Child.tagName() == "MENUVOLUME") {
                QString tempStringVolume=Child.firstChild().toText().data();
                int tempIntVolume=tempStringVolume.toInt();
                menuVolume=(tempIntVolume * MainVolume) / 100;
                ui->verticalSlider_4->setValue(tempIntVolume);

            }
            if (Child.tagName() == "ALARMVOLUME") {
                QString tempStringVolume=Child.firstChild().toText().data();
                int tempIntVolume=tempStringVolume.toInt();
                alarmVolume=(tempIntVolume * MainVolume) / 100;
                ui->verticalSlider_3->setValue(tempIntVolume);

            }
            // Read Name and value


          // Next child
          //Child = Child.nextSibling().toElement();
         //}
        } else if (ID==category && category == "PFADE"){

                /*pathXMLZeiten="/mnt/sdcard/DoorPanel/zeiten.xml";
                pathXMLTasks="/mnt/sdcard/DoorPanel/tasks.xml";*/

                if (Child.tagName() == "ZEITERFASSUNG") {
                    pathXMLZeiten=Child.firstChild().toText().data();
                }

                if (Child.tagName() == "ERINNERUNGEN") {
                    pathXMLTasks=Child.firstChild().toText().data();
                }
                }

            Child = Child.nextSibling().toElement();
          }
      }

      // Next component
      Component = Component.nextSibling().toElement();
    }







}

void MainWindow::loadXmlZE() {
    ui->Zeiterfassung_LISTE->clear();

    // The QDomDocument class represents an XML document.
    QDomDocument xmlBOM;
    // Load xml file as raw data
    QFile fxml(pathXMLZeiten);
    if (!fxml.open(QIODevice::ReadOnly)) {
      // Error while loading file
      qDebug() << "Error while loading file";
      // return 1;
    }
    // Set data into the QDomDocument before processing
    xmlBOM.setContent(&fxml);
    fxml.close();

    // Extract the root markup
    QDomElement root = xmlBOM.documentElement();

    // Get root names and attributes
    QString Type = root.tagName();
    QString DEVICE = root.attribute("DEVICE", "No name");
    QString CATEGORY = root.attribute("CATEGORY", "1900");

    // Get the first child of the root (Markup COMPONENT is expected)
    QDomElement Component = root.firstChild().toElement();

    // Loop while there is a child
    while (!Component.isNull()) {
      // Check if the child tag name is COMPONENT
      if (Component.tagName() == "ENTRY") {

        // Read and display the component ID
        QString ID = Component.attribute("ID", "No ID");

        // Get the first child of the component
        QDomElement Child = Component.firstChild().toElement();

        QString NAME;
        QString BEGINN;
        QString ENDE;

        QString TAG;
        QString MONAT;
        QString JAHR;
        QString BEGINN_H;
        QString BEGINN_M;
        QString ENDE_H;
        QString ENDE_M;

        // Read each child of the component node
        while (!Child.isNull()) {
          // Read Name and value
          if (Child.tagName() == "TAG")
            TAG = Child.firstChild().toText().data();
          if (Child.tagName() == "MONAT")
            MONAT = Child.firstChild().toText().data();
          if (Child.tagName() == "JAHR")
            JAHR = Child.firstChild().toText().data();
          if (Child.tagName() == "BEGINN_H")
            BEGINN_H = Child.firstChild().toText().data();
          if (Child.tagName() == "BEGINN_M")
            BEGINN_M = Child.firstChild().toText().data();
          if (Child.tagName() == "ENDE_H")
            ENDE_H = Child.firstChild().toText().data();
          if (Child.tagName() == "ENDE_M")
            ENDE_M = Child.firstChild().toText().data();
          // Next child
          Child = Child.nextSibling().toElement();
        }

        QListWidgetItem *eintrag = new QListWidgetItem;
        QString searchEntry = ID;
        QVariant elementDaten(searchEntry);

        int diffH = abs(BEGINN_H.toInt() - ENDE_H.toInt()); // 15 - 12 = 3

        int diffM = abs(BEGINN_M.toInt() - ENDE_M.toInt()); // 46 - 34 = 12

        double M_toH =
            static_cast<double>(diffM) / static_cast<double>(60); // 12/60 = 0,2

        double totalH = diffH + M_toH; // 3 + 0,2 = 3,2

        QString TOTAL = QString::number(totalH, 'f', 1);

        eintrag->setData(Qt::UserRole, elementDaten);
        eintrag->setText(
            " " + TAG + ". " + MONAT + ". " + JAHR +
            " ~ " /*+BEGINN_H+":"+BEGINN_M+" - "+ENDE_H+":"+ENDE_M+"/"*/ + TOTAL +
            " H");

        ui->Zeiterfassung_LISTE->addItem(eintrag);
      }

      // Next component

      Component = Component.nextSibling().toElement();
    }
    ui->slider_Zeiterfassung_Liste->setMaximum(ui->Zeiterfassung_LISTE->count() - 5);
}

void MainWindow::loadXmlER() {
    ui->Erinnerungen_Liste->clear();
    // The QDomDocument class represents an XML document.
    QDomDocument xmlBOM;
    // Load xml file as raw data
    QFile fxml(pathXMLTasks);
    if (!fxml.open(QIODevice::ReadOnly)) {
      // Error while loading file
      qDebug() << "Error while loading file";
      // return 1;
    }
    // Set data into the QDomDocument before processing
    xmlBOM.setContent(&fxml);
    fxml.close();

    // Extract the root markup
    QDomElement root = xmlBOM.documentElement();

    // Get root names and attributes
    QString Type = root.tagName();
    QString DEVICE = root.attribute("DEVICE", "No name");
    QString CATEGORY = root.attribute("CATEGORY", "1900");

    // Get the first child of the root (Markup COMPONENT is expected)
    QDomElement Component = root.firstChild().toElement();

    // Loop while there is a child
    while (!Component.isNull()) {
      // Check if the child tag name is COMPONENT
      if (Component.tagName() == "ENTRY") {

        // Read and display the component ID
        QString ID = Component.attribute("ID", "No ID");

        // Get the first child of the component
        QDomElement Child = Component.firstChild().toElement();

        QString WIEDERHOLUNG;
        QString TEXTKURZ;
        QString TEXTLANG;
        QString TAG;
        QString MONAT;
        QString JAHR;
        QString STUNDE;
        QString MINUTE;

        // Read each child of the component node
        while (!Child.isNull()) {
          // Read Name and value
          /*if (Child.tagName() == "TAG")
            TAG = Child.firstChild().toText().data();
          if (Child.tagName() == "MONAT")
            MONAT = Child.firstChild().toText().data();
          if (Child.tagName() == "JAHR")
            JAHR = Child.firstChild().toText().data();
          if (Child.tagName() == "BEGINN_H")
            BEGINN_H = Child.firstChild().toText().data();
          if (Child.tagName() == "BEGINN_M")
            BEGINN_M = Child.firstChild().toText().data();
          if (Child.tagName() == "ENDE_H")
            ENDE_H = Child.firstChild().toText().data();
          if (Child.tagName() == "ENDE_M")
            ENDE_M = Child.firstChild().toText().data();*/
          if (Child.tagName() == "TEXTKURZ")
            TEXTKURZ = Child.firstChild().toText().data();
          // Next child
          Child = Child.nextSibling().toElement();
        }

        QListWidgetItem *eintrag = new QListWidgetItem;
        QString searchEntry = ID;
        QVariant elementDaten(searchEntry);

        eintrag->setData(Qt::UserRole, elementDaten);
        eintrag->setText(TEXTKURZ);

        ui->Erinnerungen_Liste->addItem(eintrag);
      }

      // Next component

      Component = Component.nextSibling().toElement();
    }
    ui->slider_Erinnerungen_Liste->setMaximum(ui->Erinnerungen_Liste->count() - 5);
}

void MainWindow::changeXMLElementZEITEN(QString ITEM_ID, QString TAG, QString MONAT, QString JAHR, QString BEGINN_H, QString BEGINN_M, QString ENDE_H, QString ENDE_M) {


    QString leadingZero;

        int tag2 = TAG.toInt();
        int monat2 = MONAT.toInt();

        if (tag2 < 10) { TAG="0"+TAG; }
        if (monat2 < 10) { TAG="0"+MONAT; }




    QFile *fxml= new QFile(pathXMLZeiten);
        if (!fxml->open((QIODevice::ReadWrite | QIODevice::Text))) {
            qDebug() << "Error while loading file";
        }

    QByteArray xmlData(fxml->readAll());
    QDomDocument xmlBOM("SETTINGS");
        xmlBOM.setContent(xmlData);
        QDomElement root = xmlBOM.documentElement();
        QString Type = root.tagName();
        QDomElement Component = root.firstChild().toElement();

        while (!Component.isNull()) {

            if (Component.tagName() == "ENTRY") {

                QString ID = Component.attribute("ID", "");
                QDomElement Child = Component.firstChild().toElement();

                if (Component.attribute( "ID", "" ) == ITEM_ID) {

                    while (!Child.isNull()) {

                        if (Child.tagName()=="TAG") {
                            Child.firstChild().setNodeValue(TAG);
                        }
                        if (Child.tagName()=="MONAT") {
                            Child.firstChild().setNodeValue(MONAT);
                        }
                        if (Child.tagName()=="JAHR") {
                            Child.firstChild().setNodeValue(JAHR);
                        }
                        if (Child.tagName()=="BEGINN_H") {
                            Child.firstChild().setNodeValue(BEGINN_H);
                        }
                        if (Child.tagName()=="BEGINN_M") {
                            Child.firstChild().setNodeValue(BEGINN_M);
                        }
                        if (Child.tagName()=="ENDE_H") {
                            Child.firstChild().setNodeValue(ENDE_H);
                        }
                        if (Child.tagName()=="ENDE_M") {
                            Child.firstChild().setNodeValue(ENDE_M);
                        }

                        Child = Child.nextSibling().toElement();
                    }
                }
            }
            Component = Component.nextSibling().toElement();
        }




    fxml->resize(0);
    QTextStream stream(fxml);
    stream.setDevice(fxml);
    xmlBOM.save(stream, 4);
    fxml->close();

}


void MainWindow::deleteXMLElementZEITEN(QString ITEM_ID)
{

    QFile *fxml= new QFile(pathXMLZeiten);
        if (!fxml->open((QIODevice::ReadWrite | QIODevice::Text))) {
            qDebug() << "Error while loading file";
        }


    QByteArray xmlData(fxml->readAll());
    QDomDocument xmlBOM("SETTINGS");
        xmlBOM.setContent(xmlData);

        QDomElement root = xmlBOM.documentElement();

        QString Type = root.tagName();

        QDomElement Component = root.firstChild().toElement();


        while (!Component.isNull()) {

          if (Component.tagName() == "ENTRY") {

            QString ID = Component.attribute("ID", "");


            QDomElement Child = Component.firstChild().toElement();

            QString NAME;
            QString BEGINN;
            QString ENDE;
            QString TAG;
            QString MONAT;
            QString JAHR;
            QString BEGINN_H;
            QString BEGINN_M;
            QString ENDE_H;
            QString ENDE_M;
            if (Component.attribute( "ID", "" ) == ITEM_ID) {
                qDebug() << ITEM_ID;
                root.removeChild(Component);
            }

            while (!Child.isNull()) {
              Child = Child.nextSibling().toElement();
            }


          }
          Component = Component.nextSibling().toElement();
        }

    fxml->resize(0);
    QTextStream stream(fxml);
    stream.setDevice(fxml);
    xmlBOM.save(stream, 4);
    fxml->close();

}


void MainWindow::saveXMLSettings() {

        QString leadingZero;



        QFile *fxml= new QFile(pathXMLSettings);
            if (!fxml->open((QIODevice::ReadWrite | QIODevice::Text))) {
                qDebug() << "Error while loading file";
            }

        QByteArray xmlData(fxml->readAll());
        QDomDocument xmlBOM("SETTINGS");
            xmlBOM.setContent(xmlData);
            QDomElement root = xmlBOM.documentElement();
            QString Type = root.tagName();
            QDomElement Component = root.firstChild().toElement();

            while (!Component.isNull()) {

                if (Component.tagName() == "ENTRY") {

                    QString ID = Component.attribute("ID", "");
                    QDomElement Child = Component.firstChild().toElement();

                    if (Component.attribute( "ID", "" ) == "AUDIO") {

                        while (!Child.isNull()) {

                            if (Child.tagName()=="MAINVOLUME") {
                                Child.firstChild().setNodeValue(QString::number(ui->verticalSlider_5->value()));
                            }
                            if (Child.tagName()=="MENUVOLUME") {
                                Child.firstChild().setNodeValue(QString::number(ui->verticalSlider_4->value()));
                            }
                            if (Child.tagName()=="ALARMVOLUME") {
                                Child.firstChild().setNodeValue(QString::number(ui->verticalSlider_3->value()));
                            }

                            Child = Child.nextSibling().toElement();
                        }
                    }
                }
                Component = Component.nextSibling().toElement();
            }




        fxml->resize(0);
        QTextStream stream(fxml);
        stream.setDevice(fxml);
        xmlBOM.save(stream, 4);
        fxml->close();


}

void MainWindow::on_pushButton_3_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();

  // QString asdf = audio_beep.toString();

  ui->TITEL->setText("ZEITERFASSUNG");
  ui->NUMBER_TITEL->setText(ui->NUMBER_ZEITERFASSUNG->text());
  ui->INHALT->setCurrentWidget(ui->TAB_ZEITERFASSUNG);
  ui->tabWidget->setCurrentWidget(ui->TAB_ZEITERFASSUNG_LISTE);
}

void MainWindow::on_pushButton_5_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->TITEL->setText("ERINNERUNGEN");
  ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_LISTE);
  ui->NUMBER_TITEL->setText(ui->NUMBER_ERINNERUNGEN->text());
  ui->INHALT->setCurrentWidget(ui->TAB_ERINNERUNGEN);
}

void MainWindow::on_pushButton_7_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->TITEL->setText("KALENDER");
  ui->NUMBER_TITEL->setText(ui->NUMBER_KALENDER->text());
  ui->INHALT->setCurrentWidget(ui->TAB_KALENDER);

  // set names for the header controls.
  // prevMonth->setObjectName(QLatin1String("qt_calendar_prevmonth"));
  // nextMonth->setObjectName(QLatin1String("qt_calendar_nextmonth"));
  // monthButton->setObjectName(QLatin1String("qt_calendar_monthbutton"));
  // yearButton->setObjectName(QLatin1String("qt_calendar_yearbutton"));
  // yearEdit->setObjectName(QLatin1String("qt_calendar_yearedit"));

  // QToolButton *tools =
  // calendar->findChild<QToolButton*>(QLatin1String("qt_calendar_prevmonth"));
}

void MainWindow::on_pushButton_12_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  loadXmlZE();
  ui->slider_Zeiterfassung_Liste->setMaximum(ui->Zeiterfassung_LISTE->count() - 5);
  ui->tabWidget->setCurrentWidget(ui->TAB_ZEITERFASSUNG_LISTE);
}

void MainWindow::on_pushButton_4_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  QTime time1 = QTime::currentTime();
  ui->tabWidget->setCurrentWidget(ui->TAB_ZEITERFASSUNG_NEU);
  ui->spin_ZE_dd_neu->setValue(QDate::currentDate().toString("dd").toInt());
  ui->spin_ZE_MM_neu->setValue(QDate::currentDate().toString("MM").toInt());
  ui->spin_ZE_yy_neu->setValue(QDate::currentDate().toString("yyy").toInt());

  ui->spin_ZE_h_beginn_neu->setValue(time1.toString("hh").toInt());
  ui->spin_ZE_m_beginn_neu->setValue(time1.toString("mm").toInt());

  ui->spin_ZE_h_ende_neu->setValue(time1.toString("hh").toInt() + 1);
  ui->spin_ZE_m_ende_neu->setValue(time1.toString("mm").toInt());
}

void MainWindow::on_spin_ZE_dd_plus_clicked() {
    player1->setVolume(menuVolume);
    player1->setMedia(audio_denied);
    player1->play();
}

void MainWindow::on_spin_ZE_dd_minus_clicked() {
    player1->setVolume(menuVolume);
    player1->setMedia(audio_denied);
    player1->play();
}

void MainWindow::on_spin_ZE_MM_plus_clicked() {
    player1->setVolume(menuVolume);
    player1->setMedia(audio_denied);
    player1->play();
}

void MainWindow::on_spin_ZE_mm_minus_clicked() {
    player1->setVolume(menuVolume);
    player1->setMedia(audio_denied);
    player1->play();
}

void MainWindow::on_spin_ZE_yy_plus_clicked() {
    player1->setVolume(menuVolume);
    player1->setMedia(audio_denied);
    player1->play();
}

void MainWindow::on_spin_ZE_yy_minus_clicked() {
    player1->setVolume(menuVolume);
    player1->setMedia(audio_denied);
    player1->play();
}

void MainWindow::on_spin_ZE_h_beginn_plus_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_h_beginn->setValue(ui->spin_ZE_h_beginn->value() + 1);
}

void MainWindow::on_spin_ZE_h_beginn_ende_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_h_beginn->setValue(ui->spin_ZE_h_beginn->value() - 1);
}

void MainWindow::on_spin_ZE_m_beginn_plus_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_m_beginn->setValue(ui->spin_ZE_m_beginn->value() + 1);
}

void MainWindow::on_spin_ZE_m_beginn_minus_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_m_beginn->setValue(ui->spin_ZE_m_beginn->value() - 1);
}

void MainWindow::on_spin_ZE_h_ende_plus_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_h_ende->setValue(ui->spin_ZE_h_ende->value() + 1);
}

void MainWindow::on_spin_ZE_h_ende_minus_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_h_ende->setValue(ui->spin_ZE_h_ende->value() - 1);
}

void MainWindow::on_spin_ZE_m_ende_plus_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_m_ende->setValue(ui->spin_ZE_m_ende->value() + 1);
}

void MainWindow::on_spin_ZE_m_ende_minus_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_m_ende->setValue(ui->spin_ZE_m_ende->value() - 1);
}

void MainWindow::on_sliderMusikPlaylist_2_valueChanged(int value) { value = 0; }

void MainWindow::on_slider_Zeiterfassung_Liste_valueChanged(int value) {

  if (value != currentSliderValueZE) {
    ui->Zeiterfassung_LISTE->verticalScrollBar()->setValue(value);
    currentSliderValueZE = value;
  }
}

void MainWindow::on_slider_Erinnerungen_Liste_valueChanged(int value) {
  if (value != currentSliderValueER) {
    ui->Erinnerungen_Liste->verticalScrollBar()->setValue(value);
    currentSliderValueER = value;
  }
}

void MainWindow::on_button_SYSTEM_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->TITEL->setText("SYSTEM");
  ui->NUMBER_TITEL->setText(ui->NUMBER_SYSTEM->text());
  ui->INHALT->setCurrentWidget(ui->TAB_SYSTEM);
  ui->tabWidget_3->setCurrentWidget(ui->tab);
}

void MainWindow::on_button_DEAKTIVIERT_2_clicked() {
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
    redAlert->stop();
    QList<QWidget *> qwidgets = this->findChildren<QWidget *>();
    for (int i = 0; i < qwidgets.count(); i++) {
      qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_standard").toString());
    }
    redAlertAnim->stop();
    ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_ANZEIGEN);
}

void MainWindow::on_button_DEAKTIVIERT_1_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_denied);
  player1->play();
}

void MainWindow::on_pushButton_20_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  loadXmlER();
}

void MainWindow::on_pushButton_21_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_canceled);
  player1->play();
}

void MainWindow::on_pushButton_22_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
}

void MainWindow::on_pushButton_10_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  QString leadingZero;
  if (ui->spin_ZE_dd->value()<10) { leadingZero = "0"; }
  if (ui->spin_ZE_MM->value()<10) { leadingZero = "0"; }
  changeXMLElementZEITEN(leadingZero + QString::number(ui->spin_ZE_dd->value())+"."+ leadingZero + QString::number(ui->spin_ZE_MM->value())+"."+QString::number(ui->spin_ZE_yy->value()),QString::number(ui->spin_ZE_dd->value()),QString::number(ui->spin_ZE_MM->value()),QString::number(ui->spin_ZE_yy->value()),QString::number(ui->spin_ZE_h_beginn->value()),QString::number(ui->spin_ZE_m_beginn->value()),QString::number(ui->spin_ZE_h_ende->value()),QString::number(ui->spin_ZE_m_ende->value()));
  loadXmlZE();
  ui->slider_Zeiterfassung_Liste->setMaximum(ui->Zeiterfassung_LISTE->count() - 5);
  ui->tabWidget->setCurrentWidget(ui->TAB_ZEITERFASSUNG_LISTE);
}

void MainWindow::on_pushButton_11_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_canceled);
  player1->play();
  ui->tabWidget->setCurrentWidget(ui->TAB_ZEITERFASSUNG_LISTE);
}

void MainWindow::on_pushButton_clicked() {

  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->MENU->setCurrentWidget(ui->menu_2);
}

void MainWindow::on_pushButton_2_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->MENU->setCurrentWidget(ui->menu_1);
}

void MainWindow::on_pushButton_23_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();

  QListWidgetItem *currentItem = ui->Erinnerungen_Liste->currentItem();
  QString dataID;
  if (currentItem != 0) {
    QVariant data = currentItem->data(Qt::UserRole);
    dataID = data.toString();
    getXMLitem("ERINNERUNGEN",dataID,"r");
  }

  ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_ANZEIGEN);
}

void MainWindow::on_Erinnerungen_Liste_itemPressed(QListWidgetItem *item) {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
}

void MainWindow::on_slider_Erinnerungen_Liste_sliderReleased() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
}

void MainWindow::on_Erinnerungen_Liste_itemClicked(QListWidgetItem *item) {

  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
}

void MainWindow::on_Zeiterfassung_LISTE_itemClicked(QListWidgetItem *item) {

  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
}

void MainWindow::getXMLitem(QString type, QString itemID, QString action) {



  if (type=="ZEITERFASSUNG") {
  // The QDomDocument class represents an XML document.
  QDomDocument xmlBOM;
  // Load xml file as raw data
  QFile fxml(pathXMLZeiten);
  if (!fxml.open(QIODevice::ReadOnly)) {
    // Error while loading file
    qDebug() << "Error while loading file";
    // return 1;
  }
  // Set data into the QDomDocument before processing
  xmlBOM.setContent(&fxml);
  fxml.close();

  // Extract the root markup
  QDomElement root = xmlBOM.documentElement();

  // Get root names and attributes
  QString Type = root.tagName();
  QString DEVICE = root.attribute("DEVICE", "No name");
  QString CATEGORY = root.attribute("CATEGORY", "1900");

  // Get the first child of the root (Markup COMPONENT is expected)
  QDomElement Component = root.firstChild().toElement();

  // Loop while there is a child
  while (!Component.isNull()) {
    // Check if the child tag name is COMPONENT
    if (Component.tagName() == "ENTRY") {

      // Read and display the component ID
      QString ID = Component.attribute("ID", "No ID");
      ui->TITEL->setText(ID);
      //--
      if (ID == itemID) {
        // Get the first child of the component
        QDomElement Child = Component.firstChild().toElement();

        QString NAME;
        QString BEGINN;
        QString ENDE;

        QString TAG;
        QString MONAT;
        QString JAHR;
        QString BEGINN_H;
        QString BEGINN_M;
        QString ENDE_H;
        QString ENDE_M;

        // Read each child of the component node
        while (!Child.isNull()) {
          // Read Name and value
          if (Child.tagName() == "TAG")
            TAG = Child.firstChild().toText().data();
          if (Child.tagName() == "MONAT")
            MONAT = Child.firstChild().toText().data();
          if (Child.tagName() == "JAHR")
            JAHR = Child.firstChild().toText().data();
          if (Child.tagName() == "BEGINN_H")
            BEGINN_H = Child.firstChild().toText().data();
          if (Child.tagName() == "BEGINN_M")
            BEGINN_M = Child.firstChild().toText().data();
          if (Child.tagName() == "ENDE_H")
            ENDE_H = Child.firstChild().toText().data();
          if (Child.tagName() == "ENDE_M")
            ENDE_M = Child.firstChild().toText().data();
          // Next child
          Child = Child.nextSibling().toElement();
        }

        ui->spin_ZE_MM->setValue(MONAT.toInt());
        ui->spin_ZE_dd->setValue(TAG.toInt());
        ui->spin_ZE_h_beginn->setValue(BEGINN_H.toInt());
        ui->spin_ZE_h_ende->setValue(ENDE_H.toInt());
        ui->spin_ZE_m_beginn->setValue(BEGINN_M.toInt());
        ui->spin_ZE_m_ende->setValue(ENDE_M.toInt());
        ui->spin_ZE_yy->setValue(JAHR.toInt());

        break;
      }
    }

    // Next component

    Component = Component.nextSibling().toElement();
  }
  ui->tabWidget->setCurrentWidget(ui->TAB_ZEITERFASSUNG_BEARBEITEN);
  /* ********* */
  }
  else if (type=="ERINNERUNGEN") {

        if (action == "r") {
      // The QDomDocument class represents an XML document.
      QDomDocument xmlBOM;
      // Load xml file as raw data
      QFile fxml(pathXMLTasks);
      if (!fxml.open(QIODevice::ReadOnly)) {
        // Error while loading file
        qDebug() << "Error while loading file";
        // return 1;
      }
      // Set data into the QDomDocument before processing
      xmlBOM.setContent(&fxml);
      fxml.close();

      // Extract the root markup
      QDomElement root = xmlBOM.documentElement();

      // Get root names and attributes
      QString Type = root.tagName();
      QString DEVICE = root.attribute("DEVICE", "No name");
      QString CATEGORY = root.attribute("CATEGORY", "1900");


      // Get the first child of the root (Markup COMPONENT is expected)
      QDomElement Component = root.firstChild().toElement();

      // Loop while there is a child
      while (!Component.isNull()) {
        // Check if the child tag name is COMPONENT
        if (Component.tagName() == "ENTRY") {

          // Read and display the component ID
          QString ID = Component.attribute("ID", "No ID");
          if (ID == itemID) {

          // Get the first child of the component
          QDomElement Child = Component.firstChild().toElement();

          QString WIEDERHOLUNG;
          QString TEXTKURZ;
          QString TEXTLANG;
          QString TAG;
          QString MONAT;
          QString JAHR;
          QString STUNDE;
          QString MINUTE;
          QString TAGNAME;

          // Read each child of the component node
          while (!Child.isNull()) {
            // Read Name and value
            if (Child.tagName() == "WIEDERHOLUNG")
                WIEDERHOLUNG = Child.firstChild().toText().data();
            if (Child.tagName() == "TEXTLANG")
                TEXTLANG = Child.firstChild().toText().data();
            if (Child.tagName() == "TEXTKURZ")
              TEXTKURZ = Child.firstChild().toText().data();
            if (Child.tagName() == "TAG")
              TAG = Child.firstChild().toText().data();
            if (Child.tagName() == "MONAT")
              MONAT = Child.firstChild().toText().data();
            if (Child.tagName() == "JAHR")
              JAHR = Child.firstChild().toText().data();
            if (Child.tagName() == "STUNDE")
              STUNDE = Child.firstChild().toText().data();
            if (Child.tagName() == "MINUTE")
              MINUTE = Child.firstChild().toText().data();
            if (Child.tagName() == "TAGNAME")
              TAGNAME = Child.firstChild().toText().data();
            // Next child
            Child = Child.nextSibling().toElement();
          }

          ui->ERINNERUNG_Label_TEXTKURZ->setText(TEXTKURZ);
          ui->ERINNERUNG_Label_DATUM->setText(TAG+". "+MONAT+". "+JAHR);
          ui->ERINNERUNG_Label_ZEIT->setText(STUNDE+":"+MINUTE);
          ui->ERINNERUNG_Text->setText(TEXTLANG);
          if (WIEDERHOLUNG == "D") { WIEDERHOLUNG = "TÄGLICH"; ui->ERINNERUNG_Label_DATUM->setText("");}
          else if (WIEDERHOLUNG == "W") { WIEDERHOLUNG = "WÖCHENTLICH"; ui->ERINNERUNG_Label_DATUM->setText(TAGNAME); }
          else if (WIEDERHOLUNG == "M") { WIEDERHOLUNG = "MONATLICH"; ui->ERINNERUNG_Label_DATUM->setText("AM "+TAG+".");}
          else if (WIEDERHOLUNG == "J") { WIEDERHOLUNG = "JÄHRLICH"; ui->ERINNERUNG_Label_DATUM->setText(TAG+". "+MONAT+".");}
          else if (WIEDERHOLUNG == "E") { WIEDERHOLUNG = "EINMALIG"; }

          ui->ERINNERUNG_Label_REPEAT->setText(WIEDERHOLUNG);

          ui->redAlertText->setText(TEXTKURZ);

          break;
        }
      }

        // Next component

        Component = Component.nextSibling().toElement();
      }
    } else if (action == "a") {

        }
  }
}

void MainWindow::on_pushButton_6_clicked() {
  // QListWidgetItem *currentItem = ui->ExplorerViewMusic->currentItem();
  QListWidgetItem *currentItem = ui->Zeiterfassung_LISTE->currentItem();
  QString dataID;
  if (currentItem != 0) {
    QVariant data = currentItem->data(Qt::UserRole);
    dataID = data.toString();
    getXMLitem("ZEITERFASSUNG",dataID);
  }
}

void MainWindow::on_spin_ZE_dd_plus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_dd_neu->setValue(ui->spin_ZE_dd_neu->value() + 1);
}

void MainWindow::on_spin_ZE_dd_minus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_dd_neu->setValue(ui->spin_ZE_dd_neu->value() - 1);
}

void MainWindow::on_spin_ZE_MM_plus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_MM_neu->setValue(ui->spin_ZE_MM_neu->value() + 1);
}

void MainWindow::on_spin_ZE_mm_minus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  //ui->spin_ZE_mm_minus_neu->setValue(ui->spin_ZE_mm_minus_neu->value() - 1);
}

void MainWindow::on_spin_ZE_yy_plus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_yy_neu->setValue(ui->spin_ZE_yy_neu->value() + 1);
}

void MainWindow::on_spin_ZE_yy_minus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_yy_neu->setValue(ui->spin_ZE_yy_neu->value() - 1);
}

void MainWindow::on_spin_ZE_h_beginn_plus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_h_beginn_neu->setValue(ui->spin_ZE_h_beginn_neu->value() + 1);
}

void MainWindow::on_spin_ZE_h_beginn_ende_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_h_beginn_neu->setValue(ui->spin_ZE_h_beginn_neu->value() - 1);
}

void MainWindow::on_spin_ZE_m_beginn_plus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_m_beginn_neu->setValue(ui->spin_ZE_m_beginn_neu->value() + 1);
}

void MainWindow::on_spin_ZE_m_beginn_minus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_m_beginn_neu->setValue(ui->spin_ZE_m_beginn_neu->value() - 1);

}

void MainWindow::on_spin_ZE_h_ende_plus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_h_ende_neu->setValue(ui->spin_ZE_h_ende_neu->value() + 1);
}

void MainWindow::on_spin_ZE_h_ende_minus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_h_ende_neu->setValue(ui->spin_ZE_h_ende_neu->value() - 1);
}

void MainWindow::on_spin_ZE_m_ende_plus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_m_ende_neu->setValue(ui->spin_ZE_m_ende_neu->value() + 1);
}

void MainWindow::on_spin_ZE_m_ende_minus_neu_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->spin_ZE_m_ende_neu->setValue(ui->spin_ZE_m_ende_neu->value() - 1);
}

void MainWindow::on_pushButton_15_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();

  // NEU SPEICHERN
  writeXML();
  loadXmlZE();
  ui->slider_Zeiterfassung_Liste->setMaximum(ui->Zeiterfassung_LISTE->count() - 5);
  ui->tabWidget->setCurrentWidget(ui->TAB_ZEITERFASSUNG_LISTE);

}

void MainWindow::writeXML() {
    //QString pathXMLZeiten("/home/ankalagon/Dokumente/DoorPanel/tasks_test.xml");

    // Load xml file as raw data
    QFile fxml(pathXMLZeiten);
    if (!fxml.open(QIODevice::ReadWrite)) {
      // Error while loading file
      qDebug() << "Error while loading file";
      // return 1;
    }

    QFile fxml2(pathXMLZeiten);
    if (!fxml2.open(QIODevice::ReadOnly)) {
      // Error while loading file
      qDebug() << "Error while loading file";
      // return 1;
    }

    QString TAG;
    QString MONAT;
    QString JAHR;
    QString BEGINN_H;
    QString BEGINN_M;
    QString ENDE_H;
    QString ENDE_M;
    if (ui->spin_ZE_dd_neu->value() < 10) {
        TAG = "0" + QString::number(ui->spin_ZE_dd_neu->value());
    } else {
        TAG=QString::number(ui->spin_ZE_dd_neu->value());
    }
    if (ui->spin_ZE_MM_neu->value() < 10) {
        MONAT = "0" + QString::number(ui->spin_ZE_MM_neu->value());
    } else {
        MONAT=QString::number(ui->spin_ZE_dd_neu->value());
    }

    JAHR = QString::number(ui->spin_ZE_yy_neu->value());
    BEGINN_H = QString::number(ui->spin_ZE_h_beginn_neu->value());
    BEGINN_M = QString::number(ui->spin_ZE_m_beginn_neu->value());
    ENDE_H = QString::number(ui->spin_ZE_h_ende_neu->value());
    ENDE_M = QString::number(ui->spin_ZE_m_ende_neu->value());


    QXmlStreamWriter writer(&fxml);

    QXmlStreamReader reader(&fxml2);
    while(!reader.atEnd())
    {
      if(reader.isStartDocument())
        writer.writeStartDocument();

      if(reader.isStartElement())
      {
        writer.writeStartElement(reader.name().toString());
        writer.writeAttributes(reader.attributes());

        // New elements are appended here
        if(reader.name() == "SETTINGS")
        {
          writer.writeStartElement("ENTRY");
          writer.writeAttribute("ID",TAG+"."+MONAT+"."+JAHR);
          writer.writeTextElement("TAG",TAG);
          writer.writeTextElement("MONAT",MONAT);
          writer.writeTextElement("JAHR",JAHR);
          writer.writeTextElement("BEGINN_H",BEGINN_H);
          writer.writeTextElement("BEGINN_M",BEGINN_M);
          writer.writeTextElement("ENDE_H",ENDE_H);
          writer.writeTextElement("ENDE_M",ENDE_M);

          writer.writeEndElement();
        }
      }

      if(reader.isCharacters())
        writer.writeCharacters(reader.text().toString());

      if(reader.isEndElement())
        writer.writeEndElement();

      if(reader.isEndDocument())
        writer.writeEndElement();

      reader.readNext();
    }
}

void MainWindow::on_pushButton_13_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_canceled);
  player1->play();
  //deleteXMLElementZEITEN("17.01.2017");
  //changeXMLElementZEITEN();
}

void MainWindow::on_pushButton_14_clicked() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
  ui->tabWidget->setCurrentWidget(ui->TAB_ZEITERFASSUNG_LISTE);
}

void MainWindow::on_slider_Zeiterfassung_Liste_sliderReleased() {
  player1->setVolume(menuVolume);
  player1->setMedia(audio_beep);
  player1->play();
}


MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pushButton_17_clicked()
{

}

void MainWindow::on_pushButton_16_clicked()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
    ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_LISTE);
}

void MainWindow::on_redAlertText_clicked()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
    redAlert->stop();
    QList<QWidget *> qwidgets = this->findChildren<QWidget *>();
    for (int i = 0; i < qwidgets.count(); i++) {
      qwidgets[i]->setStyleSheet(qwidgets[i]->property("css_standard").toString());
    }
    redAlertAnim->stop();
    ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_ANZEIGEN);
}

void MainWindow::on_pushButton_19_clicked()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
    loadXmlZE();
}

void MainWindow::on_pushButton_8_clicked()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
    QListWidgetItem *currentItem = ui->Zeiterfassung_LISTE->currentItem();
    QString dataID;
    if (currentItem != 0) {
      QVariant data = currentItem->data(Qt::UserRole);
      dataID = data.toString();
      deleteXMLElementZEITEN(dataID);
    }
    loadXmlZE();
    ui->slider_Zeiterfassung_Liste->setMaximum(ui->Zeiterfassung_LISTE->count() - 5);

}



void MainWindow::on_pushButton_38_clicked()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
    ui->tabWidget_3->setCurrentWidget(ui->AUDIO);
}

void MainWindow::on_pushButton_9_clicked()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
    loadXmlZE();
    ui->slider_Zeiterfassung_Liste->setMaximum(ui->Zeiterfassung_LISTE->count() - 5);
}

void MainWindow::on_verticalSlider_5_valueChanged(int value)
{
    if (loadingSettings == false) {
    MainVolume = value;
    menuVolume = (value * ui->verticalSlider_4->value()) / 100;
    alarmVolume = (value * ui->verticalSlider_3->value()) / 100;

    player1->setVolume(menuVolume);
    redAlert->setVolume(alarmVolume);
    loadingSettings = true;
    saveXMLSettings();
    loadingSettings = false;
    }
}

void MainWindow::on_verticalSlider_4_valueChanged(int value)
{
        if (loadingSettings == false) {
    menuVolume = (value * MainVolume) / 100;

    player1->setVolume(menuVolume);
    loadingSettings = true;
    saveXMLSettings();
    loadingSettings = false;
        }
}

void MainWindow::on_verticalSlider_3_valueChanged(int value)
{

        if (loadingSettings == false) {
    alarmVolume = (value * MainVolume) / 100;

    redAlert->setVolume(alarmVolume);
    loadingSettings = true;
    saveXMLSettings();
    loadingSettings = false;
        }
}

void MainWindow::on_pushButton_27_clicked()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
    ui->tabWidget_2->setCurrentWidget(ui->TAB_ERINNERUNGEN_NEU);
}

void MainWindow::on_pushButton_48_clicked()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
    loadingSettings = true;
    loadSettings("AUDIO");
    loadSettings("PFADE");



    ui->Zeiterfassung_LISTE->clear();
    ui->Erinnerungen_Liste->clear();
    loadXmlER();
    loadXmlZE();
    ui->slider_Zeiterfassung_Liste->setMaximum(ui->Zeiterfassung_LISTE->count() - 5);
    ui->slider_Erinnerungen_Liste->setMaximum(ui->Erinnerungen_Liste->count() - 5);

    loadingSettings = false;
}

void MainWindow::on_verticalSlider_3_sliderReleased()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
}

void MainWindow::on_verticalSlider_4_sliderReleased()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
}

void MainWindow::on_verticalSlider_5_sliderReleased()
{
    player1->setVolume(menuVolume);
    player1->setMedia(audio_beep);
    player1->play();
}
