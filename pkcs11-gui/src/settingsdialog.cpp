#include "inc/settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include <QDir>

#include <QVariant>
#include <QList>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    connect(ui->addDriverBtn, &QToolButton::clicked,[this](auto){
        QString driverFile = QFileDialog::getOpenFileName(this,tr("Driver file"),QDir::homePath(),QString("Library files(*.so)"));
        if (!driverFile.isEmpty()){
            QList<QString> emptyList;
            QStringList currentDrivers= this->settings.value("drivers",qVariantFromValue(emptyList)).toStringList();
            if (!currentDrivers.contains(driverFile)){
                currentDrivers.append(driverFile);
                this->settings.setValue("drivers",qVariantFromValue(currentDrivers));
                this->refreshDrivers();
            }
        }
    });
    connect (ui->delDriverBtn,&QToolButton::clicked,[this](auto){
        if (!ui->driverTable->selectedItems().isEmpty()){
            QList<QString> emptyList;
            QStringList currentDrivers= this->settings.value("drivers",qVariantFromValue(emptyList)).toStringList();
            currentDrivers.removeAll(ui->driverTable->selectedItems().at(0)->text());
            this->settings.setValue("drivers",qVariantFromValue(currentDrivers));
            this->refreshDrivers();
        }
    });
    this->refreshDrivers();
}

void SettingsDialog::refreshDrivers(){
     QList<QString> emptyList;
     QList<QString> currentDrivers= this->settings.value("drivers",qVariantFromValue(emptyList)).toStringList();
     ui->driverTable->clearContents();
     ui->driverTable->setRowCount(currentDrivers.size());
     for (int i=0;i<currentDrivers.size();i++){
         ui->driverTable->setItem(i,0,new QTableWidgetItem(currentDrivers.at(i)));
     }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
