/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this
 * template
 */

/*
 * File:   dataloader.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "loader/dataset.h"
#include "tensor/xtensor_lib.h"

using namespace std;

template <typename DType, typename LType>
class DataLoader {
 public:
  Dataset<DType, LType>* ptr_dataset;
  int batch_size;
  bool shuffle;
  bool drop_last;
  xt::xarray<int> indices;
  int num_batches;

  DataLoader(Dataset<DType, LType>* ptr_dataset, int batch_size,
             bool shuffle = true, bool drop_last = false, int seed = -1)
      : ptr_dataset(ptr_dataset),
        batch_size(batch_size),
        shuffle(shuffle),
        drop_last(drop_last) {
    // cout << "hi0" << endl;

    // Initialize indices
    int num_samples = ptr_dataset->len();
    indices = xt::arange<int>(num_samples);

    // cout << num_samples << endl;
    // cout << batch_size << endl;
    // Shuffle if required
    if (shuffle) {
      if (seed >= 0) {
        xt::random::shuffle(indices);

      } else {
        xt::random::shuffle(indices);
      }
    }

    // cout << "hi1" << endl;

    if (drop_last == true) {
      num_batches = num_samples / batch_size;
    } else {
      if (num_samples % batch_size == 0) {
        num_batches = num_samples / batch_size;
      } else if (num_samples % batch_size <= batch_size) {
        num_batches = num_samples / batch_size;
      } else {
        num_batches = num_samples / batch_size + 1;
      }
    }

    if (batch_size > num_samples) {
      num_batches = 0;
    }

    // cout << num_batches << endl;
  }

  virtual ~DataLoader() {}

  // New getBatch() function to fetch the batch of data and labels
  Batch<DType, LType> getBatch(int batch_index) const {
    // Fetch the dataset shape
    if (batch_index < 0 || batch_index >= num_batches) {
      throw out_of_range("Batch index out of range");
    }
    // cout << "hi" << endl;

    int start_index = batch_index * batch_size;
    int end_index = start_index + batch_size;

    if (batch_index == num_batches - 1 && !drop_last) {
      end_index = ptr_dataset->len();
    }

    xt::svector<unsigned long> data_shape = ptr_dataset->get_data_shape();
    xt::svector<unsigned long> label_shape = ptr_dataset->get_label_shape();

    data_shape[0] = end_index - start_index;
    label_shape[0] = end_index - start_index;

    TensorDataset<DType, LType>* tensor_dataset =
        dynamic_cast<TensorDataset<DType, LType>*>(ptr_dataset);
    xt::xarray<DType> batch_data(data_shape);

    xt::xarray<LType> batch_label;
    if (label_shape.size() != 0 && label_shape[0] != 0) {
      batch_label = xt::xarray<LType>(label_shape);
    } else {
      // cout << "rong ne" << endl;
      batch_label = xt::xarray<LType>{};
    }

    for (int i = start_index; i < end_index; ++i) {
      int shuffled_index = indices(i);
      // cout << shuffled_index << endl;
      DataLabel<DType, LType> sample = ptr_dataset->getitem(shuffled_index);

      // Fill batch data
      xt::view(batch_data, i - start_index, xt::all()) = sample.getData();

      // handle empty label
      if (label_shape.size() != 0 && label_shape[0] != 0) {
        xt::view(batch_label, i - start_index, xt::all()) = sample.getLabel();
      }
    }

    return Batch<DType, LType>(batch_data, batch_label);
  }

  class Iterator {
   private:
    DataLoader<DType, LType>* ptr_loader;
    int current_index;

   public:
    Iterator(DataLoader<DType, LType>* Data, int index)
        : ptr_loader(Data), current_index(index) {

          };
    Iterator& operator=(const Iterator& iterator) {
      if (this != &iterator) {
        ptr_loader = iterator.ptr_loader;
        current_index = iterator.current_index;
      }
      return *this;
    }
    Batch<DType, LType> operator*() {
      return ptr_loader->getBatch(current_index);
    }
    bool operator!=(const Iterator& iterator) {
      return current_index != iterator.current_index;
    }
    // Prefix ++ overload
    Iterator& operator++() {
      ++current_index;
      return *this;
    }
    // Postfix ++ overload
    Iterator operator++(int) {
      Iterator iterator = *this;
      ++*this;
      return iterator;
    }
  };

  // begin()
  Iterator begin() { return Iterator(this, 0); }

  Iterator end() { return Iterator(this, num_batches); }

  // New method: from V2: begin
  int get_batch_size() { return batch_size; }
  int get_sample_count() { return ptr_dataset->len(); }
  int get_total_batch() { return int(ptr_dataset->len() / batch_size); }
};
#endif /* DATALOADER_H */
