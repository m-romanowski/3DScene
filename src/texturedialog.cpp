#include "texturedialog.h"
#include "ui_texturedialog.h"

TextureDialog::TextureDialog(QString filename, Color color, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextureDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    if(!filename.isEmpty())
    {
        this->filename = filename;

        QPixmap texture(this->filename);
        ui->preview->setPixmap(texture.scaled(ui->preview->width(), ui->preview->height()));

        texturePixmap = texture;
    }
    else
    {
        this->color = color;

        QPixmap background(ui->preview->width(), ui->preview->height());
        background.fill(QColor(this->color.getRed(), this->color.getGreen(), this->color.getBlue(), this->color.getAlpha()));
        ui->preview->setPixmap(background);

        texturePixmap = background;
    }
}

TextureDialog::~TextureDialog()
{
    delete ui;
}

void TextureDialog::on_textureWindowExitButton_clicked()
{
    this->close();
}

void TextureDialog::on_loadFileButton_clicked()
{
    filename = QFileDialog::getOpenFileName(this,
            tr("Load a texture"), "",
            tr("File format (*.jpg *.jpeg *.bmp)"));

    if(filename.isEmpty())
    {
        return;
    }
    else
    {
        QPixmap pixmap(filename);
        width = pixmap.width();
        height = pixmap.height();
        ui->preview->setPixmap(pixmap.scaled(ui->preview->width(), ui->preview->height()));

        texturePixmap = pixmap;
    }
}

void TextureDialog::on_loadColorButton_clicked()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Choose a color");
    QPixmap pixmap(ui->preview->width(), ui->preview->height());

    if(color.isValid())
    {
        pixmap.fill(color);
        this->color = Color(color.red(), color.green(), color.blue(), color.alpha());
    }
    else
    {
        pixmap.fill(QColor(this->color.getRed(), this->color.getGreen(), this->color.getBlue(), this->color.getAlpha()));
    }

    this->filename = "";
    texturePixmap = pixmap;
    ui->preview->setPixmap(pixmap);
}

QPixmap TextureDialog::getTexture()
{
    return texturePixmap;
}

QString TextureDialog::getFilename()
{
    return filename;
}

Color TextureDialog::getColor()
{
    return color;
}

int TextureDialog::getWidth()
{
    return width;
}

int TextureDialog::getHeight()
{
    return height;
}
