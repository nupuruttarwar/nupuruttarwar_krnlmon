/*
 * Copyright 2013-present Barefoot Networks, Inc.
 * Copyright (c) 2022 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "switch_types_int.h"
#include "switch_pd_routing.h"
#include "switch_nhop.h"

#ifndef __SWITCH_NHOP_INT_H__
#define __SWITCH_NHOP_INT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SWITCH_NHOP_HASH_SEED 0x12345678

// The toal maximum number of unique neighbors - swicthes or servers
// that can connect to a switch
#ifdef P4_TRANSIENT_LOOP_PREVENTION_ENABLE
#ifndef MAX_NEIGHBOR_NODES
#define MAX_NEIGHBOR_NODES 24
#endif
#else
#define MAX_NEIGHBOR_NODES 0
#endif


// note - have relevant functions now in switch_nhop.c
#define switch_nhop_handle_create(_device, _count) \
  switch_handle_create(                    \
      _device, SWITCH_HANDLE_TYPE_NHOP, sizeof(switch_nhop_info_t))

#define switch_ecmp_group_handle_create(_device, _count) \
  switch_handle_create(                    \
      _device, SWITCH_HANDLE_TYPE_ECMP_GROUP, sizeof(switch_ecmp_info_t))

#define switch_ecmp_group_handle_delete(_device, _count) \
  switch_handle_delete(                    \
      _device, SWITCH_HANDLE_TYPE_ECMP_GROUP, _handle)

#define switch_nhop_handle_delete(_device, _handle, _count) \
  switch_handle_delete(_device, SWITCH_HANDLE_TYPE_NHOP, _handle)

#define switch_ecmp_handle_delete_contiguous(_device, _handle, count) \
  switch_handle_delete_contiguous(                                    \
      _device, SWITCH_HANDLE_TYPE_NHOP, _handle, count)

#define switch_nhop_get(_device, _handle, _info)                    \
  ({                                                                \
    switch_nhop_info_t *_tmp_nhop_info = NULL;                      \
    (void)(_tmp_nhop_info == *_info);                               \
    switch_handle_get(                                              \
        _device, SWITCH_HANDLE_TYPE_NHOP, _handle, (void **)_info); \
  })

#define switch_ecmp_member_handle_create(_device) \
  switch_handle_create(                           \
      _device, SWITCH_HANDLE_TYPE_ECMP_MEMBER, sizeof(switch_ecmp_member_t))

#define switch_ecmp_member_handle_delete(_device, _handle) \
  switch_handle_delete(_device, SWITCH_HANDLE_TYPE_ECMP_MEMBER, _handle)

#define switch_ecmp_member_get(_device, _handle, _info)                    \
  ({                                                                       \
    switch_ecmp_member_t *_tmp_ecmp_member_info = NULL;                    \
    (void)(_tmp_ecmp_member_info == *_info);                               \
    switch_handle_get(                                                     \
        _device, SWITCH_HANDLE_TYPE_ECMP_MEMBER, _handle, (void **)_info); \
  })

#define switch_ecmp_group_get(_device, _handle, _info)                    \
  ({                                                                \
    switch_ecmp_info_t *_tmp_ecmp_info = NULL;                      \
    (void)(_tmp_ecmp_info == *_info);                               \
    switch_handle_get(                                              \
        _device, SWITCH_HANDLE_TYPE_ECMP_GROUP, _handle, (void **)_info); \
  })

#define switch_ecmp_handle_delete(_device, _handle) \
  switch_handle_delete(_device, SWITCH_HANDLE_TYPE_ECMP_GROUP, _handle)

#define SWITCH_ECMP_MEMBER_INIT(_m)                         \
  do {                                                      \
    if (_m) {                                               \
      SWITCH_MEMSET(_m, 0x0, sizeof(switch_ecmp_member_t)); \
      _m->nhop_handle = SWITCH_API_INVALID_HANDLE;          \
    }                                                       \
  } while (0);

#define SWITCH_NHOP_HASH_KEY_SIZE sizeof(switch_nhop_key_t)

#define SWITCH_NHOP_KEY_GET(_api_nhop_info, _nhop_key)                    \
  do {                                                                    \
    SWITCH_MEMSET(&_nhop_key, 0x0, sizeof(switch_nhop_key_t));            \
    if (_api_nhop_info->nhop_type == SWITCH_NHOP_TYPE_IP) {               \
      SWITCH_ASSERT(SWITCH_RIF_HANDLE(_api_nhop_info->rif_handle));       \
      _nhop_key.handle = _api_nhop_info->rif_handle;                      \
    } else if (_api_nhop_info->nhop_type == SWITCH_NHOP_TYPE_TUNNEL) {    \
      SWITCH_ASSERT(SWITCH_TUNNEL_HANDLE(_api_nhop_info->tunnel_handle)); \
      _nhop_key.handle = _api_nhop_info->tunnel_handle;                   \
      _nhop_key.vni = _api_nhop_info->tunnel_vni;                         \
    } else if (_api_nhop_info->nhop_type == SWITCH_NHOP_TYPE_MPLS) {      \
      _nhop_key.handle = _api_nhop_info->mpls_handle;                     \
    } else if (_api_nhop_info->nhop_type == SWITCH_NHOP_TYPE_DROP) {      \
      _nhop_key.handle = SWITCH_RIF_HANDLE(_api_nhop_info->rif_handle)    \
                             ? _api_nhop_info->rif_handle                 \
                             : _api_nhop_info->intf_handle;               \
    } else {                                                              \
      _nhop_key.handle = _api_nhop_info->intf_handle;                     \
    }                                                                     \
    SWITCH_MEMCPY(&_nhop_key.ip_addr,                                     \
                  &api_nhop_info->ip_addr,                                \
                  sizeof(switch_ip_addr_t));                              \
  } while (0);

typedef enum switch_nhop_pd_action_s {
  SWITCH_NHOP_PD_ACTION_NON_TUNNEL = 0x1,
  SWITCH_NHOP_PD_ACTION_TUNNEL = 0x2,
  SWITCH_NHOP_PD_ACTION_MGID_TUNNEL = 0x3,
  SWITCH_NHOP_PD_ACTION_FLOOD = 0x4,
  SWITCH_NHOP_PD_ACTION_GLEAN = 0x5,
  SWITCH_NHOP_PD_ACTION_DROP = 0x6,
  SWITCH_NHOP_PD_ACTION_MAX
} switch_nhop_pd_action_t;

static inline char *switch_nhop_type_to_string(switch_nhop_type_t nhop_type) {
  switch (nhop_type) {
    case SWITCH_NHOP_TYPE_NONE:
      return "punt";
    case SWITCH_NHOP_TYPE_IP:
      return "ip";
    case SWITCH_NHOP_TYPE_TUNNEL:
      return "tunnel";
    case SWITCH_NHOP_TYPE_MPLS:
      return "mpls";
    case SWITCH_NHOP_TYPE_GLEAN:
      return "glean";
    case SWITCH_NHOP_TYPE_DROP:
      return "drop";
    default:
      return "unknown";
  }
}

typedef enum switch_nhop_flags_s {
  SWITCH_NHOP_MARK_TO_BE_DELETED = 1 << 0,
} switch_nhop_flags_t;

/** single path nexthop struct */
typedef struct switch_spath_info_s {
  /**
   * nhop key programmed by the application and
   * acts as the key for nhop hashtable. This should
   * be the first entry in this struct for hashing
   */
  switch_nhop_key_t nhop_key;

  /** nhop application info */
  switch_api_nhop_info_t api_nhop_info;

  /** nhop handle - self pointer */
  switch_handle_t nhop_handle;

  /** neighbor handle */
  switch_handle_t neighbor_handle;

  /** interface ifindex */
  switch_ifindex_t ifindex;

  /** replication ID */
  switch_rid_t rid;

  /** is tunnel */
  bool tunnel;

} switch_spath_info_t;

typedef struct switch_nhop_info_s {
  /** hashtable node */
  switch_hashnode_t node;

  /** nhop type - nhop/wcmp/ecmp */
  switch_nhop_id_type_t id_type;

  /** nhop handle - self pointer */
  switch_handle_t nhop_handle;

  /** nhop type is single path */
  switch_spath_info_t spath;
  
  /** number of ecmp references */
  switch_uint32_t ecmp_ref_count;

  /** List of ecmp-members(handles) referring this nexthop */
  Pvoid_t PJLarr_ecmp_members;

  /** nexthop reference count */
  switch_handle_t nhop_ref_count;

  /** nhop flags */
  switch_uint32_t flags;

  switch_pd_routing_info_t switch_pd_routing_info;
  /** Number of contiguous nexthop groups */
  unsigned int num_nhop_groups;
} switch_nhop_info_t;

/** nhop device context */
typedef struct switch_nhop_context_s {
  /** nexthop hashtable */
  switch_hashtable_t nhop_hashtable;

} switch_nhop_context_t;

#define SWITCH_NHOP_SPATH_INFO(nhop) nhop->spath

#define SWITCH_NHOP_ID_TYPE_ECMP(nhop)          \
  (nhop->id_type == SWITCH_NHOP_ID_TYPE_ECMP || \
   nhop->id_type == SWITCH_NHOP_ID_TYPE_WCMP)

#define SWITCH_NHOP_NUM_ECMP_MEMBER_REF(nhop) nhop->ecmp_ref_count

#define SWITCH_NHOP_ECMP_MEMBER_REF_LIST(nhop) nhop->PJLarr_ecmp_members

#define SWITCH_NHOP_TYPE(_nhop_info) _nhop_info->spath.api_nhop_info.nhop_type

switch_status_t switch_nhop_init(switch_device_t device);

switch_status_t switch_nhop_free(switch_device_t device);

switch_status_t switch_nhop_default_entries_add(switch_device_t device);

switch_status_t switch_nhop_default_entries_delete(switch_device_t device);

switch_status_t switch_ecmp_member_activate(switch_device_t device,
                                            switch_handle_t ecmp_handle,
                                            switch_uint16_t num_nhops,
                                            switch_handle_t *nhop_handles,
                                            bool activate);

switch_status_t switch_nhop_ecmp_members_deactivate(
    switch_device_t device, switch_nhop_info_t *nhop_info);

switch_status_t switch_nhop_update(switch_device_t device,
                                   switch_handle_t nhop_handle,
                                   switch_ifindex_t ifindex,
                                   switch_port_lag_index_t port_lag_index,
                                   switch_nhop_pd_action_t pd_action);

switch_status_t switch_nhop_ecmp_type_get(switch_device_t device,
                                          switch_handle_t ecmp_handle,
                                          bool *ecmp_type);

/**
 * Adds created ecmp member to the list of members
 * maintained by nhop and increases ecmp ref count by 1
 */
switch_status_t switch_nhop_ecmp_member_list_add(
    switch_device_t device,
    switch_nhop_info_t *nhop_info,
    switch_handle_t ecmp_mem_handle);

switch_status_t switch_nhop_ecmp_member_list_remove(
    switch_device_t device,
    switch_nhop_info_t *nhop_info,
    switch_handle_t ecmp_mem_handle);


#ifdef __cplusplus
}
#endif

#endif /* __SWITCH_NHOP_INT_H__ */
