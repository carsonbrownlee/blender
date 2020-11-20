/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "BKE_mesh.h"
#include "BKE_persistent_data_handle.hh"
#include "DNA_mesh_types.h"
#include "DNA_meshdata_types.h"
#include "DNA_pointcloud_types.h"

#include "node_geometry_util.hh"

static bNodeSocketTemplate geo_node_point_instance_in[] = {
    {SOCK_GEOMETRY, N_("Geometry")},
    {SOCK_OBJECT, N_("Object")},
    {-1, ""},
};

static bNodeSocketTemplate geo_node_point_instance_out[] = {
    {SOCK_GEOMETRY, N_("Geometry")},
    {-1, ""},
};

namespace blender::nodes {

static void add_instances_from_geometry_component(InstancesComponent &instances,
                                                  const GeometryComponent &src_geometry,
                                                  const AttributeDomain domain,
                                                  Object *object)
{
  Float3ReadAttribute positions = src_geometry.attribute_get_for_read<float3>(
      "Position", domain, {0, 0, 0});
  FloatReadAttribute radii = src_geometry.attribute_get_for_read<float>("Radius", domain, 1.0f);

  for (const int i : IndexRange(positions.size())) {
    const float radius = radii[i];
    instances.add_instance(object, positions[i], {0, 0, 0}, {radius, radius, radius});
  }
}

static void geo_point_instance_exec(GeoNodeExecParams params)
{
  GeometrySet geometry_set = params.extract_input<GeometrySet>("Geometry");

  bke::PersistentObjectHandle object_handle = params.extract_input<bke::PersistentObjectHandle>(
      "Object");
  Object *object = params.handle_map().lookup(object_handle);

  if (object != nullptr) {
    InstancesComponent &instances = geometry_set.get_component_for_write<InstancesComponent>();
    if (geometry_set.has<MeshComponent>()) {
      add_instances_from_geometry_component(instances,
                                            *geometry_set.get_component_for_read<MeshComponent>(),
                                            ATTR_DOMAIN_VERTEX,
                                            object);
    }
    if (geometry_set.has<PointCloudComponent>()) {
      add_instances_from_geometry_component(
          instances,
          *geometry_set.get_component_for_read<PointCloudComponent>(),
          ATTR_DOMAIN_POINT,
          object);
    }
  }

  geometry_set.remove<MeshComponent>();
  geometry_set.remove<PointCloudComponent>();
  params.set_output("Geometry", std::move(geometry_set));
}
}  // namespace blender::nodes

void register_node_type_geo_point_instance()
{
  static bNodeType ntype;

  geo_node_type_base(&ntype, GEO_NODE_POINT_INSTANCE, "Point Instance", 0, 0);
  node_type_socket_templates(&ntype, geo_node_point_instance_in, geo_node_point_instance_out);
  ntype.geometry_node_execute = blender::nodes::geo_point_instance_exec;
  nodeRegisterType(&ntype);
}
