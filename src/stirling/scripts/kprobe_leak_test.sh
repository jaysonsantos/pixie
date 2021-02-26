#!/bin/bash

# NOTE: This test must be run as root.
# If you run it as a regular user, it will use sudo and prompt for sudo access.
#
# This test checks that Stirling cleans up after itself after it receives
# signals such as SIGTERM and SIGINT. In particular, it is important that
# Stirling not leave any attached kprobes, as these won't be cleaned up by
# the kernel.
#
# Note: the following command can be used to remove all kprobes, if needed.
#    sudo sh -c 'echo > /sys/kernel/debug/tracing/kprobe_events'
#
# Perf buffers and perf events are assigned file descriptors,
# so hopefully the kernel gets rid of them when the process dies, right?

# TODO(oazizi): Switch to looking for the "Probes successfully deployed" message.
Tstart=10

# Amount of time after start, after which we kill the executable.
Tkill=3

if [ $# -eq 0 ]; then
  echo "Usage: $0 <command to test for leaks>"
  echo "Example: ./kprobe_leak_test.sh \$(bazel info bazel-bin)/src/stirling/binaries/stirling_wrapper"
  exit 1
fi

test_cmd=$1

# Switch to root user.
if [[ $EUID -ne 0 ]]; then
   sudo "$0" "$test_cmd"
   exit
fi

echo "Program to test: $test_cmd"

if [ -z "$test_cmd" ]; then
  echo "Error: no command to test"
  exit 1
fi

num_tests=0
num_passed=0

function test() {
    num_tests=$((num_tests+1))

    signal=$1

    echo "---------------"
    echo "Testing $signal"

    $test_cmd > /dev/null &
    if [ $? -ne 0 ]; then
        echo "FAILED: Cannot run program"
        return 1
    fi
    pid=$!
    echo "Program PID: $pid"
    pixie_probe_tag=__pixie__$pid

    sleep $Tstart
    num_probes=$(grep -c $pixie_probe_tag /sys/kernel/debug/tracing/kprobe_events)
    echo "Number of probes while running: $num_probes"

    if [ "$num_probes" -eq 0 ]; then
      echo "Test FAILED: Expecting kprobes to be deployed. Test infra cannot be trusted."
      return 1
    fi

    # Delayed kill
    sh -c "sleep $Tkill && kill -$signal $pid" &

    # Wait for process to terminate.
    wait $pid

    num_probes=$(grep -c $pixie_probe_tag /sys/kernel/debug/tracing/kprobe_events)
    echo "Final number of probes: $num_probes"

    if [ "$num_probes" -ne 0 ]; then
      echo "Test FAILED: Program is leaking BPF probes"
      return 1
    fi

    echo "Test PASSED"
    num_passed=$((num_passed+1))
    return 0
}

echo "Running a test with KILL, which should leak probes. This is test infra sanity check."
test KILL
if [ $num_passed -eq 1 ]; then
  echo "Test appears broken. KILL should cause probes to leak."
fi

# Reset the test stats.
num_tests=0
num_passed=0

test TERM
test HUP
test INT
test QUIT

echo "--------------"
echo "Success rate: $num_passed / $num_tests"
if [ $num_passed -ne $num_tests ]; then
    echo "Some tests FAILED"
else
    echo "All tests PASSED"
fi

exit
