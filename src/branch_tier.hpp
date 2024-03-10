#pragma once

#include "branch2.hpp"

#include "toml.hpp"

#include <vector>

class BranchTier
{
public:
  BranchTier();
  BranchTier(toml::value const &,
             uint64_t const min_free_space = 0);

public:
  std::vector<Branch2>::iterator begin() { return _branches.begin(); }
  std::vector<Branch2>::iterator end() { return _branches.end(); }
  std::vector<Branch2>::const_iterator begin() const { return _branches.begin(); }
  std::vector<Branch2>::const_iterator end() const { return _branches.end(); }

private:
  std::vector<Branch2> _branches;
};
