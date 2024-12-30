/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this
 * template
 */

/*
 * File:   Softmax.cpp
 * Author: ltsach
 *
 * Created on August 25, 2024, 2:46 PM
 */

#include "layer/Softmax.h"

#include <filesystem>  //require C++17

#include "ann/functions.h"
#include "sformat/fmt_lib.h"
namespace fs = std::filesystem;

Softmax::Softmax(int axis, string name) : m_nAxis(axis) {
  if (trim(name).size() != 0)
    m_sName = name;
  else
    m_sName = "Softmax_" + to_string(++m_unLayer_idx);
}

Softmax::Softmax(const Softmax& orig) {}

Softmax::~Softmax() {}

xt::xarray<double> Softmax::forward(xt::xarray<double> X) {
  // TODO YOUR CODE IS HERE
  m_aCached_Y = softmax(X, m_nAxis);

  return m_aCached_Y;
}

xt::xarray<double> Softmax::backward(xt::xarray<double> DY) {
  xt::xarray<double> DX = xt::zeros_like(DY);
  int batch_size = DY.shape()[0];

  // Loop over each sample in the batch
  for (int i = 0; i < batch_size; ++i) {
    xt::xarray<double> Y_sample = xt::view(m_aCached_Y, i);
    xt::xarray<double> DY_sample = xt::view(DY, i);

    xt::xarray<double> Y_outer =
        xt::linalg::outer(Y_sample, xt::transpose(Y_sample));
    xt::xarray<double> Y_diag = xt::diag(Y_sample);
    xt::xarray<double> J_Y = Y_diag - Y_outer;

    xt::view(DX, i) = xt::linalg::dot(J_Y, DY_sample);
  }

  return DX;
}

string Softmax::get_desc() {
  string desc = fmt::format("{:<10s}, {:<15s}: {:4d}", "Softmax",
                            this->getname(), m_nAxis);
  return desc;
}
