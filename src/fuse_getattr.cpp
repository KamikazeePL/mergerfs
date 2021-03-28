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

#include "state.hpp"

#include "fs_inode.hpp"
#include "symlinkify.hpp"
#include "ugid.hpp"

#include "fuse.h"


namespace FUSE::GETATTR
{
  int
  getattr(const char      *fusepath_,
          struct stat     *st_,
          fuse_timeouts_t *timeout_)
  {
    State s;
    int rv;
    const fuse_context *fc = fuse_get_context();
    const ugid::Set     ugid(fc->uid,fc->gid);

    rv = s->getattr(fusepath_,st_,timeout_);
    if(rv < 0)
      return rv;

    if(s->symlinkify && symlinkify::can_be_symlink(*st_,s->symlinkify_timeout))
      st_->st_mode = symlinkify::convert(st_->st_mode);

    fs::inode::calc(fusepath_,st_);

    timeout_->entry = ((rv >= 0) ?
                       s->entry_cache_timeout :
                       s->neg_entry_cache_timeout);
    timeout_->attr  = s->attr_cache_timeout;

    return 0;
  }
}
