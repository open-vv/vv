#include "gtest/gtest.h"
#include <iostream>
#include <QApplication>
int main(int argc, char** argv){
  QApplication* q=new QApplication ( argc, argv );
  ::testing::InitGoogleTest(&argc, argv);
  
  int a=RUN_ALL_TESTS();
  delete q;
  return a;
}
