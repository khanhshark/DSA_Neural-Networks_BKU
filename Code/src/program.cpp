#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include <filesystem>  //require C++17
namespace fs = std::filesystem;

#include <iostream>

#include "ann/annheader.h"
#include "config/Config.h"
#include "dataset/DSFactory.h"
#include "heap/Heap.h"
#include "list/listheader.h"
#include "loader/dataloader.h"
#include "loader/dataset.h"
#include "modelzoo/threeclasses.h"
#include "modelzoo/twoclasses.h"
#include "sformat/fmt_lib.h"
#include "tensor/xtensor_lib.h"
#include "util/ArrayLib.h"
#include "util/Point.h"
#include "util/sampleFunc.h"

using namespace std;

/* ******* BEGIN MLP ******* */
void mlpDemo1() {
  xt::random::seed(42);
  DSFactory factory("./config.txt");
  xmap<string, TensorDataset<double, double>*>* pMap =
      factory.get_datasets_2cc();
  TensorDataset<double, double>* train_ds = pMap->get("train_ds");
  TensorDataset<double, double>* valid_ds = pMap->get("valid_ds");
  TensorDataset<double, double>* test_ds = pMap->get("test_ds");
  DataLoader<double, double> train_loader(train_ds, 50, true, false);
  DataLoader<double, double> valid_loader(valid_ds, 50, false, false);
  DataLoader<double, double> test_loader(test_ds, 50, false, false);

  cout << "Train dataset: " << train_ds->len() << endl;
  cout << "Valid dataset: " << valid_ds->len() << endl;
  cout << "Test dataset: " << test_ds->len() << endl;

  int nClasses = 3;
  ILayer* layers[] = {new FCLayer(2, 50, true), new ReLU(),
                      new FCLayer(50, nClasses, true), new Softmax()};

  MLPClassifier model("./config.txt", "2c-classification", layers,
                      sizeof(layers) / sizeof(ILayer*));

  SGD optim(2e-3);
  CrossEntropy loss;
  ClassMetrics metrics(nClasses);

  model.compile(&optim, &loss, &metrics);
  model.fit(&train_loader, &valid_loader, 1);
}

int main(int argc, char** argv) {
  mlpDemo1();
  return 0;
}
