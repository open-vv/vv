#include "gtest/gtest.h"
#include <iostream>
#include <QApplication>
int main(int argc, char** argv){
  QApplication* q=new QApplication ( argc, argv );
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
