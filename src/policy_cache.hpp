/*
  ISC License

  Copyright (c) 2019, Antonio SJ Musumeci <trapexit@spawn.link>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#pragma once

#include "nonstd/optional.hpp"
#include "boost/unordered/concurrent_flat_map.hpp"
#include "wyhash.h"
#include "strpool.h"

#include "fmt/core.h"

#include <cstdint>
#include <string>


class PolicyCache
{
public:
  typedef boost::concurrent_flat_map<uint64_t,const char*> Map;

public:
  PolicyCache(unsigned const max_size_ = 256)
    : _max_size(max_size_)
  {
    strpool_config_t config = strpool_default_config;

    strpool_init(&_strpool,&config);
  }

  ~PolicyCache()
  {
    strpool_term(&_strpool);
  }

public:
  void
  insert(std::string const &key_,
         std::string const &val_)
  {
    uint64_t hash;
    STRPOOL_U64 str;
    const char *ptr;

    fmt::print("insert {}={}\n",
               key_,
               val_);

    hash = wyhash(key_.c_str(),key_.size(),0xdeadbeef,_wyp);
    val = 
    _cache.insert_or_assign(hash,val_);
  }

  const
  nonstd::optional<std::string>
  find(std::string const &key_)
  {
    uint64_t hash;
    nonstd::optional<std::string> rv;

    hash = wyhash(key_.c_str(),key_.size(),0xdeadbeef,_wyp);
    _cache.cvisit(hash,
                  [&](Map::value_type const &v_)
                  {
                    fmt::print("get {}={}\n",
                               key_,
                               v_.second);
                    rv = v_.second;
                  });

    return rv;
  }

private:
  unsigned  _max_size;
  Map       _cache;
  strpool_t _strpool;
};
