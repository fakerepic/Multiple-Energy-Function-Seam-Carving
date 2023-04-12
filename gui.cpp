#include "gui.h"
#include <iostream>
#include "seam_carving.h"

gui::gui(QWidget* parent) : QMainWindow(parent) {
  this->setMinimumSize(500, 400);
  this->setMaximumSize(500, 400);
  imageLabel = new QLabel(this);

  openButton = new QPushButton("Open", this);
  saveButton = new QPushButton("Save", this);
  halfButton = new QPushButton("HalfSize", this);
  methodBox = new QComboBox(this);
  seamNumBox = new QSpinBox(this);
  resetButton = new QPushButton("Reset", this);
  energyButton = new QPushButton("ToggleEg", this);
  originalButton = new QPushButton("ToggleOld", this);
  directionBox = new QComboBox(this);
  seamButton = new QPushButton("Seam", this);

  openButton->setGeometry(0, 0, 100, 30);
  saveButton->setGeometry(100, 0, 100, 30);
  halfButton->setGeometry(200, 0, 100, 30);
  methodBox->setGeometry(300, 0, 100, 30);
  seamNumBox->setGeometry(400, 2, 100, 28);
  resetButton->setGeometry(0, 30, 100, 30);
  energyButton->setGeometry(100, 30, 100, 30);
  originalButton->setGeometry(200, 30, 100, 30);
  directionBox->setGeometry(300, 30, 100, 30);
  seamButton->setGeometry(400, 30, 100, 30);

  seamNumBox->setValue(50);
  seamNumBox->setMinimum(1);
  seamNumBox->setMaximum(200);
  energyButton->setChecked(false);
  methodBox->addItem("Forward");
  methodBox->addItem("Sobel");
  methodBox->addItem("Scharr");
  methodBox->addItem("Prewitt");
  methodBox->addItem("Roberts");
  methodBox->addItem("Laplacian");
  directionBox->addItem("Vertical");
  directionBox->addItem("Horizontal");

  connect(openButton, &QPushButton::clicked, this, &gui::on_open_triggered);
  connect(saveButton, &QPushButton::clicked, this, &gui::on_save_triggered);
  connect(halfButton, &QPushButton::clicked, this, &gui::on_halfSize_triggered);
  connect(resetButton, &QPushButton::clicked, this, &gui::on_reset_triggered);
  connect(energyButton, &QPushButton::clicked, this, &gui::on_energy_toggle);
  connect(originalButton, &QPushButton::clicked, this, &gui::on_original_toggle);
  connect(seamButton, &QPushButton::clicked, this, &gui::on_seam_triggered);
  connect(methodBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &gui::on_method_selected);
  connect(directionBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &gui::on_direction_selected);
}

// private slots : --------------------------------------------

void gui::on_open_triggered() {
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp, *jpeg)"));
  if (fileName.isEmpty()) return;
  seamCarver.readImage(fileName);
  imageType = Result;
  showImage(seamCarver.image());
}

void gui::on_save_triggered() {
  if (this->NullImageWarning()) return;

  QString fileName =
      QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Image Files (*.png *.jpg *.bmp, *jpeg)"));
  if (fileName.isEmpty()) return;
  seamCarver.saveImage(fileName);
}

void gui::on_seam_triggered() {
  if (this->NullImageWarning()) return;

  seamButton->setEnabled(false);
  halfButton->setEnabled(false);

  if (imageType == Original) imageType = Result;

  for (int i = 0; i < seamNumBox->value(); ++i) {
    seamButton->setText(QString::number(i + 1) + "/" + QString::number(seamNumBox->value()));
    (seamDirec == Horizontal) ? seamCarver.run_horizontal() : seamCarver.run();
    (imageType == Energy) ? showImage(seamCarver.energy()) : showImage(seamCarver.result());
  }

  seamButton->setText("Seam");
  seamButton->setEnabled(true);
  halfButton->setEnabled(true);
}

void gui::on_energy_toggle() {
  if (this->NullImageWarning()) return;

  imageType = imageType == Energy ? Result : Energy;
  (imageType == Energy) ? showImage(seamCarver.energy()) : showImage(seamCarver.result());
}

void gui::on_original_toggle() {
  if (this->NullImageWarning()) return;

  imageType = imageType == Original ? Result : Original;
  (imageType == Original) ? showImage(seamCarver.image()) : showImage(seamCarver.result());
}

void gui::on_reset_triggered() {
  seamCarver.reset();
  showImage(seamCarver.result());
}

void gui::on_halfSize_triggered() {
  if (this->NullImageWarning()) return;

  seamButton->setEnabled(false);
  halfButton->setEnabled(false);

  imageType = Result;

  int w = seamCarver.result().width() / 2;
  int h = seamCarver.result().height() / 2;
  for (int i = 0; i < w; ++i) {
    seamButton->setText(QString::number(i + 1) + "/" + QString::number(w + h));
    seamCarver.run();
    (imageType == Energy) ? showImage(seamCarver.energy()) : showImage(seamCarver.result());
  }
  for (int i = 0; i < h; ++i) {
    seamButton->setText(QString::number(i + 1 + w) + "/" + QString::number(w + h));
    seamCarver.run_horizontal();
    (imageType == Energy) ? showImage(seamCarver.energy()) : showImage(seamCarver.result());
  }

  seamButton->setText("Seam");
  seamButton->setEnabled(true);
  halfButton->setEnabled(true);
}

void gui::on_method_selected(int index) {
  seamCarver.setMode((SeamCarving::MODE)index);
  seamCarver.calcEnergymap();
  if (imageType == Energy) showImage(seamCarver.energy());
}

void gui::on_direction_selected(int index) { seamDirec = (Direction)index; }

// private : --------------------------------------------

void gui::showImage(const QImage& image) {
  this->setMinimumSize(qMax(image.width(), 500), image.height() + topOffset());
  this->setMaximumSize(qMax(image.width(), 500), image.height() + topOffset());

  imageLabel->setPixmap(QPixmap::fromImage(image));
  imageLabel->setGeometry((this->width() - image.width()) / 2,
                          topOffset() + (this->height() - image.height() - topOffset()) / 2, image.width(),
                          image.height());
  imageLabel->show();
  update();
  // wait 10ms for update
  QEventLoop loop;
  QTimer::singleShot(5, &loop, SLOT(quit()));
  loop.exec();
}

bool gui::NullImageWarning() {
  if (seamCarver.image().isNull()) {
    QMessageBox::warning(this, "Warning", "Please open an image first!");
    return true;
  }
  return false;
}
