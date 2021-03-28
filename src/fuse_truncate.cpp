/*
  Copyright (c) 2016, Antonio SJ Musumeci <trapexit@spawn.link>

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

#include "config.hpp"
#include "errno.hpp"
#include "fs_path.hpp"
#include "fs_truncate.hpp"
#include "policy_rv.hpp"
#include "ugid.hpp"
#include "state.hpp"

#include "fuse.h"

#include "toml.hpp"

#include <string>

#include <sys/types.h>
#include <unistd.h>

using std::string;


namespace l
{
  static
  int
  get_error(const PolicyRV &prv_,
            const string   &basepath_)
  {
    for(int i = 0, ei = prv_.success.size(); i < ei; i++)
      {
        if(prv_.success[i].basepath == basepath_)
          return prv_.success[i].rv;
      }

    for(int i = 0, ei = prv_.error.size(); i < ei; i++)
      {
        if(prv_.error[i].basepath == basepath_)
          return prv_.error[i].rv;
      }

    return 0;
  }

  static
  void
  truncate_loop_core(const string &basepath_,
                     const char   *fusepath_,
                     const off_t   size_,
                     PolicyRV     *prv_)
  {
    string fullpath;

    fullpath = fs::path::make(basepath_,fusepath_);

    errno = 0;
    fs::truncate(fullpath,size_);

    prv_->insert(errno,basepath_);
  }

  static
  void
  truncate_loop(const StrVec &basepaths_,
                const char   *fusepath_,
                const off_t   size_,
                PolicyRV     *prv_)
  {
    for(size_t i = 0, ei = basepaths_.size(); i != ei; i++)
      {
        l::truncate_loop_core(basepaths_[i],fusepath_,size_,prv_);
      }
  }

  static
  int
  truncate(const Policy::Action &actionFunc_,
           const Policy::Search &searchFunc_,
           const Branches       &branches_,
           const char           *fusepath_,
           const off_t           size_)
  {
    int rv;
    PolicyRV prv;
    StrVec basepaths;

    rv = actionFunc_(branches_,fusepath_,&basepaths);
    if(rv == -1)
      return -errno;

    l::truncate_loop(basepaths,fusepath_,size_,&prv);
    if(prv.error.empty())
      return 0;
    if(prv.success.empty())
      return prv.error[0].rv;

    basepaths.clear();
    rv = searchFunc_(branches_,fusepath_,&basepaths);
    if(rv == -1)
      return -errno;

    return l::get_error(prv,basepaths[0]);
  }
}

namespace FUSE::TRUNCATE
{
  int
  truncate(const char *fusepath_,
           off_t       size_)
  {
    State s;
    const fuse_context *fc = fuse_get_context();
    const ugid::Set     ugid(fc->uid,fc->gid);

    return s->truncate(fusepath_,size_);
  }
}
