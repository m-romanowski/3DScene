#ifndef ANIMATIONDIALOG_H
#define ANIMATIONDIALOG_H

#include <QDialog>
#include <animation.h>

namespace Ui {
class AnimationDialog;
}

class AnimationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnimationDialog(QString name, QWidget *parent = 0);
    ~AnimationDialog();

    Animation getAnimation();

private slots:
    void on_aTransXValue_valueChanged(double arg1);
    void on_aTransYValue_valueChanged(double arg1);
    void on_aTransZValue_valueChanged(double arg1);
    void on_aRotXValue_valueChanged(double arg1);
    void on_aRotYValue_valueChanged(double arg1);
    void on_aRotZValue_valueChanged(double arg1);
    void on_aExitButton_clicked();

private:
    Ui::AnimationDialog *ui;
    Animation animation;
    QString name;
};

#endif // ANIMATIONDIALOG_H
