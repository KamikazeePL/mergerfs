#pragma once

#include "branch2.hpp"

#include "toml.hpp"

#include <vector>

class BranchTier
{
public:
  BranchTier();
  BranchTier(toml::value const &);

public:
  uint64_t min_free_space;

public:
  std::vector<Branch2>::iterator begin() { return _branches.begin(); }
  std::vector<Branch2>::iterator end() { return _branches.end(); }

  std::vector<Branch2>::iterator cbegin() const { return _branches.cbegin(); }
  std::vector<Branch2>::iterator cend() const { return _branches.cend(); }
  

private:
  std::vector<Branch2>  _branches;
};
