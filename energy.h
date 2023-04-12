// calculates the energy of a given QImage
// return an image with the energy values as the grayscale
// all values are normalized to 0-255

#pragma once
#include <QImage>

class Energy {
public:
  // operators
  static int SobelX[3][3], SobelY[3][3];
  static int ScharrX[3][3], ScharrY[3][3];
  static int PrewittX[3][3], PrewittY[3][3];
  static int RobertsX[3][3], RobertsY[3][3];
  static int Laplacian[3][3];

  // convolution both direction and mix the results by taking average
  static void conv_mix(const QImage& image, QImage& output, const int kernelX[3][3], const int kernelY[3][3]);

  // convolution with a 3x3 kernel
  static void convolution(const QImage& image, QImage& output, const int kernel[3][3]);

  // Improved Seam Carving Using Forward Energy
  // reference: https://andrewdcampbell.github.io/seam-carving
  static void forward(const QImage& image, QImage& output);
};
