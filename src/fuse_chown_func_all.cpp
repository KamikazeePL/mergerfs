/*
  ISC License

  Copyright (c) 2022, Antonio SJ Musumeci <trapexit@spawn.link>

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

#include "fuse_chown_func_all.hpp"
#include "fuse_chown_err.hpp"

#include "fs_lchown.hpp"


FUSE::CHOWN::FuncALL::FuncALL(const toml::value &toml_)
  : _branches(toml_)
{

}

int
FUSE::CHOWN::FuncALL::operator()(const gfs::path &fusepath_,
                                 const uid_t      uid_,
                                 const gid_t      gid_)
{
  Err rv;
  gfs::path fusepath;
  gfs::path fullpath;

  for(const auto &branch_group : _branches)
    {
      for(const auto &branch : branch_group)
        {
          fullpath = branch.path / fusepath_;

          rv = fs::lchown(fullpath,uid_,gid_);
        }
    }

  return rv;
}
