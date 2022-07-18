#ifndef TEXTUREDIALOG_H
#define TEXTUREDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QPainter>
#include <QRect>
#include <QPixmap>
#include <QMouseEvent>
#include <QDebug>

#include <color.h>

namespace Ui {
class TextureDialog;
}

class TextureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextureDialog(QString filename, Color color, QWidget *parent = 0);
    ~TextureDialog();

    QString filename;
    int width, height;
    Color color;

    // Publiczne metody:
    QPixmap getTexture();
    QString getFilename();
    Color getColor();
    int getWidth();
    int getHeight();

private slots:
    //void paintEvent(QPaintEvent *event);
    //void mousePressEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
    //void mouseReleaseEvent(QMouseEvent *event);
    void on_textureWindowExitButton_clicked();
    void on_loadFileButton_clicked();
    void on_loadColorButton_clicked();

protected:
    //virtual void keyPressEvent(QKeyEvent *event);

private:
    Ui::TextureDialog *ui;
    QPixmap texturePixmap;
};

#endif // TEXTUREDIALOG_H
