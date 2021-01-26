/*
 * Copyright 2011-2020 Blender Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __IMAGE_VDB__
#define __IMAGE_VDB__

#ifdef WITH_OPENVDB
#  include <openvdb/openvdb.h>
#endif
#ifdef WITH_NANOVDB
#  include <nanovdb/util/GridHandle.h>
#endif
#ifdef WITH_OPENVKL
#   include <openvkl/openvkl.h>
#   include <openvkl/vdb_util/OpenVdbGrid.h>
#endif

#include "render/image.h"

CCL_NAMESPACE_BEGIN

class VDBImageLoader : public ImageLoader {
 public:
  VDBImageLoader(const string &grid_name);
  ~VDBImageLoader();

  virtual bool load_metadata(ImageMetaData &metadata) override;

  virtual bool load_pixels(const ImageMetaData &metadata,
                           void *pixels,
                           const size_t pixels_size,
                           const bool associate_alpha) override;

  virtual string name() const override;

  virtual bool equals(const ImageLoader &other) const override;

  virtual void cleanup() override;

  virtual bool is_vdb_loader() const override;

#ifdef WITH_OPENVDB
  openvdb::GridBase::ConstPtr get_grid();
#endif
#ifdef WITH_OPENVKL
  VKLSampler getVKLSampler() { return vklSampler; }
#endif

 protected:
  string grid_name;
#ifdef WITH_OPENVDB
  openvdb::GridBase::ConstPtr grid;
  openvdb::CoordBBox bbox;
#endif
#ifdef WITH_NANOVDB
  nanovdb::GridHandle<> nanogrid;
#endif
#ifdef WITH_OPENVKL
  openvdb::FloatGrid::Ptr vklgrid;
  std::unique_ptr<openvkl::vdb_util::OpenVdbFloatGrid> vklOpenVdbFloatGrid;
  VKLVolume vklVolumeVdb{nullptr};
  VKLSampler vklSampler{nullptr};
#endif
};

CCL_NAMESPACE_END

#endif /* __IMAGE_VDB__ */
