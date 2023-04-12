// gui for seam carving visualization
#include <QComboBox>
#include <QEventLoop>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include "seam_carving.h"

class gui : public QMainWindow {
  Q_OBJECT
public:
  explicit gui(QWidget* parent = nullptr);
  ~gui() = default;

private:
  int topOffset() const { return 60; }
  enum Direction { VERTICAL, HORIZONTAL } seamDirec = VERTICAL;
  enum ImageType { RESULT, ORIGINAL, ENERGY } imageType = RESULT;
  // algorithm and data
  SeamCarving seamCarver;

  // widgets
  QLabel*      imageLabel;
  QPushButton* openButton;
  QPushButton* saveButton;
  QPushButton* seamButton;
  QPushButton* resetButton;
  QPushButton* halfButton;
  QPushButton* originalButton;
  QPushButton* energyButton;
  QSpinBox*    seamNumBox;
  QComboBox*   methodBox;
  QComboBox*   directionBox;

private slots:
  // slots
  void on_open_triggered();
  void on_save_triggered();
  void on_seam_triggered();
  void on_reset_triggered();
  void on_halfSize_triggered();
  void on_energy_toggle();
  void on_original_toggle();
  void on_method_selected(int index);
  void on_direction_selected(int index);

private:
  // utils
  void showImage(const QImage& image);
  bool NullImageWarning();
};
