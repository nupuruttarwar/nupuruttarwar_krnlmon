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

#include "switchlink_utils.h"

uint32_t ipv4_prefix_len_to_mask(uint32_t prefix_len) {
  return (prefix_len ? (((uint32_t)0xFFFFFFFF) << (32 - prefix_len)) : 0);
}

struct in6_addr ipv6_prefix_len_to_mask(uint32_t prefix_len) {
  struct in6_addr mask;
  memset(&mask, 0, sizeof(mask));
  krnlmon_assert(prefix_len <= 128);

  int i;
  for (i = 0; i < 4; i++) {
    if (prefix_len > 32) {
      mask.s6_addr32[i] = 0xFFFFFFFF;
    } else {
      mask.s6_addr32[i] = htonl(ipv4_prefix_len_to_mask(prefix_len));
      break;
    }
    prefix_len -= 32;
  }
  return mask;
}
