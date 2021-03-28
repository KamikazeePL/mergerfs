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

#include "fuse_create_func_ff.hpp"

#include "fileinfo.hpp"
#include "fs_acl.hpp"
#include "fs_clonepath.hpp"
#include "fs_clonepath_branches.hpp"
#include "fs_open.hpp"
#include "ugid.hpp"


namespace gfs = ghc::filesystem;


FUSE::CREATE::FuncFF::FuncFF(const toml::value &toml_)
  : _branches(toml_)
{

}

int
FUSE::CREATE::FuncFF::operator()(const char       *fusepath_,
                                 const mode_t      mode_,
                                 const mode_t      umask_,
                                 fuse_file_info_t *ffi_)
{
  int rv;
  mode_t mode;
  gfs::path fusepath;
  gfs::path fullpath;

  mode = mode_;
  fusepath = &fusepath_[1];
  for(const auto &branch_group : _branches)
    {
      for(const auto &branch : branch_group)
        {
          fullpath  = branch.path / fusepath;

          rv = fs::acl::dir_has_defaults(fullpath);
          if(rv == -ENOENT)
            {
              rv = fs::clonepath_as_root(_branches,branch.path,fusepath);
              if(rv >= 0)
                rv = fs::acl::dir_has_defaults(fullpath);
            }

          if(rv >= 0)
            mode &= ~umask_;

          rv = fs::open(fullpath,ffi_->flags,mode);
          if(rv == -ENOENT)
            {
              rv = fs::clonepath_as_root(_branches,branch.path,fusepath);
              if(rv >= 0)
                {
                  rv = fs::acl::dir_has_defaults(fullpath);
                  if(rv >= 0)
                    mode &= ~umask_;
                  rv = fs::open(fullpath,ffi_->flags,mode);
                }
            }

          if(rv < 0)
            return rv;

          ffi_->fh = reinterpret_cast<uint64_t>(new FileInfo(rv,fusepath_));

          return 0;
        }
    }

  return -ENOENT;
}
