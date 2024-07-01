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

#include <absl/container/flat_hash_map.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <deque>
#include <random>

#include "src/stirling/source_connectors/socket_tracer/protocols/common/event_parser.h"
#include "src/stirling/source_connectors/socket_tracer/protocols/pulsar/test_data.h"
#include "src/stirling/source_connectors/socket_tracer/protocols/pulsar/types.h"

namespace px {
namespace stirling {
namespace protocols {
namespace pulsar {

using ::testing::ElementsAre;
using ::testing::ElementsAreArray;

class PulsarParserTest : public ::testing::Test {};

TEST_F(PulsarParserTest, ParseRaw) {
  StateWrapper state{};

  absl::flat_hash_map<int64_t, std::deque<Packet>> parsed_messages;
  ParseResult<int64_t> result =
      ParseFramesLoop(message_type_t::kRequest, kConnectData, &parsed_messages, &state);

  //   Packet expected_message0;
  //   expected_message0.msg = "\x03SELECT foo";
  //   expected_message0.sequence_id = 0;

  //   Packet expected_message1;
  //   expected_message1.msg = "\x03SELECT bar";
  //   expected_message1.sequence_id = 1;

  //   EXPECT_EQ(ParseState::kSuccess, result.state);
  //   EXPECT_THAT(parsed_messages[0], ElementsAre(expected_message0, expected_message1));
}
}  // namespace pulsar
}  // namespace protocols
}  // namespace stirling
}  // namespace px
