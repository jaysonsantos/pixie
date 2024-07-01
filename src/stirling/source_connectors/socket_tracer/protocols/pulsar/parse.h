
#pragma once

#include <string_view>

#include "src/stirling/source_connectors/socket_tracer/protocols/common/interface.h"

namespace px {
namespace stirling {
namespace protocols {

template <>
size_t FindFrameBoundary<pulsar::Message, pulsar::StateWrapper>(message_type_t /*type*/,
                                                                std::string_view buf,
                                                                pulsar::Packet*,
                                                                pulsar::StateWrapper* /*state*/);
template <>
ParseState ParseFrame<pulsar::Packet, pulsar::StateWrapper>(message_type_t type,
                                                            std::string_view* buf,
                                                            pulsar::Packet* msg,
                                                            pulsar::StateWrapper* /*state*/);

}  // namespace protocols
}  // namespace stirling
}  // namespace px
