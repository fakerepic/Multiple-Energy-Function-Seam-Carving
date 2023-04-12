#pragma once

#include <QImage>

class SeamCarving {
public:
  enum MODE { FORWARD, SOBEL, SCHARR, PREWITT, ROBERTS, LAPLACIAN } method;
  SeamCarving(MODE = FORWARD);

  void run();
  void run_horizontal();

  void readImage(const QString&);
  void saveImage(const QString&);
  void reset();
  void setMode(MODE);

  const QImage& image();
  const QImage& result();
  const QImage& energy();

public:
  void calcEnergy();

private:
  void calculateSeam();
  void removeSeam();

private:
  QImage m_image;   // original image
  QImage m_energy;  // energy map (energy map)
  QImage m_result;  // result image

  std::vector<int>              seam;     // min energy seam
  std::vector<std::vector<int>> dp_sum;   // dp array of energy sum
  std::vector<std::vector<int>> dp_from;  // dp array of min energy path
};
