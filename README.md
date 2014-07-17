# Vartrace

This is a library for storing trace information in a circular
buffer. It is designed to enable monitoring a running program with
minimal interference.

Features:

* Trace dump is self describing. That is there is no need to provide
  additional information to deserialize data.

* Data is stored in a circular buffer with minimal overhead. Service
  data is stored with each record and occupies 8 bytes. It consists of
  4 bytes timestamp, 1 byte record type id, 1 byte data type id and
  2 bytes data size.

* Mulitple records can be bundled together and written under the same
  timestamp. Such structure is treated as subtrace and can contain
  other subtraces.

* Information stored in a trace can be changed at compile time through
  log level mechanism.

* Member and stanalone functions can be used to store a non POD type
  in a trace.

* The library is designed to minimize impact on program
  execution. PODs of size 4 bytes or less are stored using assignment
  while larger types are copied by `memcpy`.

* VarTrace can be used in single threaded as well as in multithreaded
  environment. In later case one has to provide a type that will lock
  a trace object. By default no locking is done.

* Same syntax for most types: `trace.Log(kInfoLevel, message_id
  ,value);` where `value` can be POD type, array of PODs, std::vector,
  std::string, object with custom log function or anything that can be
  stored by copying `sizeof(value)` bytes.

* The code does not use external libraries and exceptions so it can be
  assembled by most compilers.

## Building



## Testing

The project uses Google test suite. To setup testing:

1. Download and install `gtest` into some directory.

2. Change directory to `Debug`.

3. Add `gtest` directory to cmake library path:
~~~~~~~~~~
export CMAKE_LIBRARY_PATH=/some/path/to/lib
~~~~~~~~~~

4. Run cmake, build tests and run them:
~~~~~~~~~~
cmake .. && make vartrace_test && valgrind ./trunk/tests/vartrace_test
~~~~~~~~~~

5. To run speed tests:
~~~~~~~~~~
cmake -DCMAKE_BUILD_TYPE=Release ..
make profile && ./trunk/tests/profile
~~~~~~~~~~

## Input data format (outdated)

Trace data consists of trace messages each of which has the
following structure:

<table>
  <tr>
    <th>Time stamp</th><th>Length</th><th>Message type</th>
	<th>Data type</th><th>Data</th>
  </tr>
  <tr>
    <th>4 bytes</th><th>2 bytes</th><th>1 byte</th>
	<th>1 byte</th><th>Length bytes</th>
  </tr>
</table>

Data field can itself be a compound structure like:

<table>
  <tr>
    <th>Length</th><th>Message type</th><th>Data type</th><th>Data</th><th>
  </tr>
  <tr>
    <th>2 bytes</th><th>1 byte</th><th>1 byte</th><th>Length bytes</th>
  </tr>
</table>

In the most general case a value can be accessed by specifying an
int array: Message type 1, Message type 2, ..., Message type N,
Array index.

All messages of some particular type must have same structure and
data types. The additional field "Data type" is needed for self
sufficiency of trace files (so they are usable even without
description file).

It is assumed that time stamp field always increasing. In other
words if the time stamp of some message bigger the one of the
following message then it means that overflow happened and time
stamp of the consequent messages should be incremented by the max
value of the time stamp column.

