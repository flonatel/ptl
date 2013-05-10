#include <ptl/object_pool.hh>

#include <gtest/gtest.h>

class ObjectPoolMTTest : public ::testing::Test {
public:
};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
