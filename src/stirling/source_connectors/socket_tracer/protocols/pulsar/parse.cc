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

#include <initializer_list>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include "src/common/base/base.h"
#include "src/stirling/source_connectors/socket_tracer/protocols/pulsar/parse.h"
#include "src/stirling/source_connectors/socket_tracer/protocols/pulsar/types.h"
#include "src/stirling/utils/binary_decoder.h"

namespace px {
namespace stirling {
namespace protocols {
namespace pulsar {

size_t FindMessageBoundary(std::string_view, size_t) {
  //   for (; start_pos < buf.size(); ++start_pos) {
  //     const char type_marker = buf[start_pos];
  //     if (type_marker == kSimpleStringMarker || type_marker == kErrorMarker ||
  //         type_marker == kIntegerMarker || type_marker == kBulkStringsMarker ||
  //         type_marker == kArrayMarker) {
  //       return start_pos;
  //     }
  //   }
  return std::string_view::npos;
}

// Redis protocol specification: https://redis.io/topics/protocol
// This can also be implemented as a recursive function.
ParseState ParseMessage(message_type_t, std::string_view* buf, Message*) {
  BinaryDecoder decoder(*buf);

  //   auto status = ParseMessage(type, &decoder, msg);

  //   if (!status.ok()) {
  //     return TranslateStatus(status);
  //   }

  //   *buf = decoder.Buf();

  return ParseState::kSuccess;
}

}  // namespace pulsar

template <>
size_t FindFrameBoundary<pulsar::Message, pulsar::StateWrapper>(message_type_t /*type*/,
                                                                std::string_view buf,
                                                                pulsar::Packet*,
                                                                pulsar::StateWrapper* /*state*/) {
  //   return pulsar::FindMessageBoundary(buf, start_pos);
  return std::string_view::npos;
}

template <>
ParseState ParseFrame<pulsar::Packet, pulsar::StateWrapper>(message_type_t type,
                                                            std::string_view* buf,
                                                            pulsar::Packet* msg,
                                                            pulsar::StateWrapper* /*state*/) {
  return pulsar::ParseMessage(type, buf, msg);
}

}  // namespace protocols
}  // namespace stirling
}  // namespace px
