#include "branch2.hpp"

#include <vector>

class Branch2Vec
{
public:
  uint64_t min_free_space;

private:
  std::vector<Branch2>  _branches;
};

class Branches2
{
public:
  uint64_t min_free_space;

private:
  std::vector<Branch2Vec> _branches;
};
