/*
 * Copyright 2018- The Pixie Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "src/common/testing/testing.h"

namespace px {
namespace stirling {
namespace protocols {
namespace pulsar {

#define D(var, str) const std::string_view var = CreateStringView<char>(str)

D(kConnectData,
  "\002\000\000\000E\000\000Z\000\000@\000@\006\000\000\177\000\000"
  "\001\177\000\000\001\374<\031\3723eYi\031\263]\a\200\030\030\353"
  "\376N\000\000\001\001\b\nL[\341_\035\230\357\326\000\000\000\""
  "\000\000\000\036\b\002\022\032\n\0062.10.1\032\000 \023*\004none"
  "R\006\b\001\020\001\030\001");

}  // namespace pulsar
}  // namespace protocols
}  // namespace stirling
}  // namespace px
