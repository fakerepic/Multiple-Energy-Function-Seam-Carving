// seam carving using qt library
#include "seam_carving.h"
#include <limits.h>
#include <iostream>
#include "energy.h"
using eg = Energy;
using namespace std::chrono;

SeamCarving::SeamCarving(SeamCarving::MODE method) : method(method) {}

void transpose(QImage& img) {
  QImage temp = QImage(img.height(), img.width(), QImage::Format_RGB32);
  for (int i = 0; i < img.width(); ++i)
    for (int j = 0; j < img.height(); ++j)
      temp.setPixel(j, i, img.pixel(i, j));
  img = temp;
}

void SeamCarving::run() {
  calcEnergymap();
  getMinEnergySeam();
  removeSeam(seam);
}

void SeamCarving::run_horizontal() {
  transpose(m_result);
  calcEnergymap();
  getMinEnergySeam();
  removeSeam(seam);
  transpose(m_result);
  transpose(m_energy);
}

void SeamCarving::calcEnergymap() {
  switch (method) {
  case Sobel: eg::conv_mix(m_result, m_energy, eg::SobelX, eg::SobelY); break;
  case Scharr: eg::conv_mix(m_result, m_energy, eg::ScharrX, eg::ScharrY); break;
  case Prewitt: eg::conv_mix(m_result, m_energy, eg::PrewittX, eg::PrewittY); break;
  case Roberts: eg::conv_mix(m_result, m_energy, eg::RobertsX, eg::RobertsY); break;
  case Laplacian: eg::convolution(m_result, m_energy, eg::Laplacian); break;
  case Forward: eg::forward(m_result, m_energy); break;
  default: break;
  }
}

void SeamCarving::getMinEnergySeam() {
  const int rows = m_result.height();
  const int cols = m_result.width();

  // init dp matrices
  dp_sum.resize(cols);
  dp_from.resize(cols);
  for (int i = 0; i < cols; ++i) {
    dp_sum[i].resize(rows);
    dp_from[i].resize(rows);
  }
  for (int i = 0; i < cols; ++i) {
    dp_sum[i][0] = qGray(m_energy.pixel(i, 0));
    dp_from[i][0] = i;
  }
  // dp
  for (int i = 1; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      int sum_left_upper = (j == 0) ? INT_MAX : dp_sum[j - 1][i - 1];
      int sum_right_upper = (j == cols - 1) ? INT_MAX : dp_sum[j + 1][i - 1];
      int sum_up = dp_sum[j][i - 1];
      int sum_min = qMin(sum_up, qMin(sum_right_upper, sum_left_upper));
      dp_from[j][i] = (sum_min == sum_up) ? j : ((sum_min == sum_left_upper) ? j - 1 : j + 1);
      dp_sum[j][i] = sum_min + qGray(m_energy.pixel(j, i));
    }
  }
  seam.resize(rows);
  // find the minimum energy col in the last row
  int min_energy_col = 0;
  int min_energy = dp_sum[0][rows - 1];
  for (int i = 1; i < cols; ++i) {
    if (dp_sum[i][rows - 1] < min_energy) {
      min_energy = dp_sum[i][rows - 1];
      min_energy_col = i;
    }
  }
  // find the seam
  seam[rows - 1] = min_energy_col;
  for (int i = rows - 2; i >= 0; --i)
    seam[i] = dp_from[seam[i + 1]][i + 1];
}

void SeamCarving::removeSeam(const std::vector<int>& seam) {
  QImage temp = QImage(m_result.width() - 1, m_result.height(), QImage::Format_RGB32);
  for (int i = 0; i < m_result.height(); ++i) {
    for (int j = 0; j < seam[i]; ++j) {
      temp.setPixel(j, i, m_result.pixel(j, i));
    }
    for (int j = seam[i] + 1; j < m_result.width(); ++j) {
      temp.setPixel(j - 1, i, m_result.pixel(j, i));
    }
  }
  m_result = temp;
}

void SeamCarving::readImage(const QString& fileName) {
  m_image = QImage(fileName);
  m_result = QImage(m_image);
  calcEnergymap();
}

void SeamCarving::saveImage(const QString& fileName) { m_result.save(fileName); }

void SeamCarving::reset() {
  m_result = QImage(m_image);
  calcEnergymap();
}
