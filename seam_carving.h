#pragma once

#include <QImage>

class SeamCarving {
public:
  enum MODE { Forward, Sobel, Scharr, Prewitt, Roberts, Laplacian,  } method;
  SeamCarving(MODE method = Forward);

  void readImage(const QString& fileName);
  void saveImage(const QString& fileName);
  void reset();
  void run();
  void run_horizontal();
  void calcEnergymap();
  void setMode(MODE mode) { method = mode; }

  const QImage& image() { return m_image; }
  const QImage& result() { return m_result; }
  const QImage& energy() {
    if (m_energy.isNull()) calcEnergymap();
    return m_energy;
  }

private:
  void getMinEnergySeam();
  void removeSeam(const std::vector<int>& seam);

private:
  QImage m_image;   // original image
  QImage m_energy;  // energy map (energy map)
  QImage m_result;  // result image

  std::vector<int>              seam;     // min energy seam
  std::vector<std::vector<int>> dp_sum;   // dp array of energy sum
  std::vector<std::vector<int>> dp_from;  // dp array of min energy path
  
  std::vector <std::vector<int>> v_energy;
};
