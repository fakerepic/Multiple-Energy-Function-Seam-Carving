#include "energy.h"
#include <array>
#include <vector>
using std::array;
using std::min_element;

// init convolution kernels
int Energy::SobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
int Energy::SobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
int Energy::ScharrX[3][3] = {{-3, 0, 3}, {-10, 0, 10}, {-3, 0, 3}};
int Energy::ScharrY[3][3] = {{-3, -10, -3}, {0, 0, 0}, {3, 10, 3}};
int Energy::PrewittX[3][3] = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
int Energy::PrewittY[3][3] = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};
int Energy::RobertsX[3][3] = {{0, 0, 0}, {0, 1, 0}, {0, 0, -1}};
int Energy::RobertsY[3][3] = {{0, 0, 0}, {0, 0, 1}, {0, -1, 0}};
int Energy::Laplacian[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};

void Energy::conv_mix(const QImage& image, QImage& output, const int (*kernelX)[3], const int (*kernelY)[3])
{
  const int col = image.width();
  const int row = image.height();

  output = QImage(col, row, QImage::Format_RGB32);
  std::vector<std::vector<int>> energy(col, std::vector<int>(row, 0));

  int max_grad = 0;
  int min_grad = std::numeric_limits<int>::max();

  for (int x = 0; x < col; ++x) {
    for (int y = 0; y < row; ++y) {
      int gx = 0;
      int gy = 0;
      for (int k = 0; k < 3; ++k) {
        for (int l = 0; l < 3; ++l) {
          int _x = qBound(0, x + k - 1, col - 1);
          int _y = qBound(0, y + l - 1, row - 1);
          int pixel = qGray(image.pixel(_x, _y));
          gx += pixel * kernelX[k][l];
          gy += pixel * kernelY[k][l];
        }
      }
      energy[x][y] = (qAbs(gx) + qAbs(gy)) / 2;
      max_grad = qMax(max_grad, energy[x][y]);
      min_grad = qMin(min_grad, energy[x][y]);
    }
  }

  // normalize to 0 - 255
  int range = max_grad - min_grad;
  if (range == 0) range = 1;
  for (int x = 0; x < col; ++x) {
    for (int y = 0; y < row; ++y) {
      int g = (energy[x][y] - min_grad) * 255 / range;
      output.setPixel(x, y, qRgb(g, g, g));
    }
  }
}

void Energy::convolution(const QImage& image, QImage& output, const int (*kernel)[3])
{
  int col = image.width();
  int row = image.height();

  output = QImage(col, row, QImage::Format_RGB32);
  std::vector<std::vector<int>> energy(col, std::vector<int>(row, 0));

  int max_grad = 0;
  int min_grad = INT_MAX;

  for (int y = 0; y < row; ++y) {
    for (int x = 0; x < col; ++x) {
      int gx = 0;
      for (int k = 0; k < 3; ++k) {
        for (int l = 0; l < 3; ++l) {
          if (kernel[k][l] == 0) continue;
          int _x = qBound(0, x + k - 1, col - 1);
          int _y = qBound(0, y + l - 1, row - 1);
          int pixel = qGray(image.pixel(_x, _y));
          gx += pixel * kernel[k][l];
        }
      }
      gx = qBound(0, qAbs(gx), 255);
      max_grad = qMax(max_grad, gx);
      min_grad = qMin(min_grad, gx);
      energy[x][y] = gx;
    }
  }

  // normalize to 0 - 255
  int range = max_grad - min_grad;
  if (range == 0) range = 1;
  for (int y = 0; y < row; ++y) {
    for (int x = 0; x < col; ++x) {
      int g = (energy[x][y] - min_grad) * 255 / range;
      output.setPixel(x, y, qRgb(g, g, g));
    }
  }
}

// reference: https://andrewdcampbell.github.io/seam-carving
void Energy::forward(const QImage& image, QImage& output)
{
  int row = image.height();
  int col = image.width();
  int max_energy = 0;
  int min_energy = std::numeric_limits<int>::max();
  output = QImage(col, row, QImage::Format_RGB32);
  /* output = image.convertToFormat(QImage::Format_Grayscale8); */

  std::vector<std::vector<int>> energy(col, std::vector<int>(row, 0));

  for (int y = 0; y < row; y++) {
    for (int x = 0; x < col; x++) {
      int up = qBound(0, y - 1, row - 1);
      int down = qBound(0, y + 1, row - 1);
      int left = qBound(0, x - 1, col - 1);
      int right = qBound(0, x + 1, col - 1);

      int cU = qAbs(qGray(image.pixel(left, y)) - qGray(image.pixel(right, y)));
      int cL = qAbs(qGray(image.pixel(x, up)) - qGray(image.pixel(left, y))) + cU;
      int cR = qAbs(qGray(image.pixel(x, up)) - qGray(image.pixel(right, y))) + cU;

      array<int, 3> cULR = {cU, cL, cR};

      int argmin = min_element(cULR.begin(), cULR.end()) - cULR.begin();
      energy[x][y] = cULR[argmin];

      min_energy = qMin(min_energy, energy[x][y]);
      max_energy = qMax(max_energy, energy[x][y]);
    }
  }

  // normalize to 0 - 255
  int range = max_energy - min_energy;
  if (range == 0) range = 1;
  for (int y = 0; y < row; y++) {
    for (int x = 0; x < col; x++) {
      int g = (energy[x][y] - min_energy) * 255 / range;
      /* output.setPixel(x, y, g); */
      output.setPixel(x, y, qRgb(g, g, g));
    }
  }
}
