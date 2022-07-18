#include "animationdialog.h"
#include "ui_animationdialog.h"

AnimationDialog::AnimationDialog(QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnimationDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    this->name = name;
    ui->aObjName->setText(this->name);
}

AnimationDialog::~AnimationDialog()
{
    delete ui;
}

void AnimationDialog::on_aTransXValue_valueChanged(double arg1)
{
    this->animation.translation.setX(arg1);
}

void AnimationDialog::on_aTransYValue_valueChanged(double arg1)
{
    this->animation.translation.setY(arg1);
}

void AnimationDialog::on_aTransZValue_valueChanged(double arg1)
{
    this->animation.translation.setZ(arg1);
}

void AnimationDialog::on_aRotXValue_valueChanged(double arg1)
{
    this->animation.rotation.setX(arg1);
}

void AnimationDialog::on_aRotYValue_valueChanged(double arg1)
{
    this->animation.rotation.setY(arg1);
}


void AnimationDialog::on_aRotZValue_valueChanged(double arg1)
{
    this->animation.rotation.setZ(arg1);
}

void AnimationDialog::on_aExitButton_clicked()
{
    this->close();
}

Animation AnimationDialog::getAnimation()
{
    return this->animation;
}
