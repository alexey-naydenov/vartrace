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

* Multiple records can be bundled together and written under the same
  timestamp. Such structure is treated as subtrace and can contain
  other subtraces.

* Information stored in a trace can be changed at compile time through
  log level mechanism.

* Member and standalone functions can be used to store a non POD type
  in a trace.

* The library is designed to minimize impact on program
  execution. PODs of size 4 bytes or less are stored using assignment
  while larger types are copied by `memcpy`.

* VarTrace can be used in single threaded as well as in multithreaded
  environment. In later case one has to provide a type that will lock
  a trace object. By default no locking is done.

* Same syntax is used to store most types: `trace.Log(kInfoLevel,
  message_id, value)` where `value` can be POD type, array of PODs,
  std::vector, std::string, object with custom log function or
  anything that can be stored by copying `sizeof(value)`
  bytes. Dynamic arrays can be logged via overloaded function:
  `trace.Log(kInfoLevel, message_id, pointer, length)`

* The code does not use external libraries and exceptions so it can be
  assembled by most compilers.

## Binary format

Each record consists of a header and a data block. The header contains
timestamp, data size, message type id, data type id and occupies 8
bytes. The data is written in machine dependent endianess. The header
is written by assigning 4 byte values so its exact structure will
depend on machine type. The table below shows the structure of a
message in little endian format:

<table>
  <tr>
    <th>Time stamp</th><th>Size (N)</th><th>Message type</th>
	<th>Data type</th><th>Data</th>
  </tr>
  <tr>
    <th>4 bytes</th><th>2 bytes</th><th>1 byte</th>
	<th>1 byte</th><th>N bytes</th>
  </tr>
</table>

If a trace is serialized on a big endian machine then the size field
will be the last entry in the header.

Data section can itself contain a trace. Subtraces dont have timestamp
field so a message has the following structure:

<table>
  <tr>
    <th>Size (N)</th><th>Message type</th><th>Data type</th><th>Data</th>
  </tr>
  <tr>
    <th>2 bytes</th><th>1 byte</th><th>1 byte</th><th>N bytes</th>
  </tr>
</table>


## Examples

The following code illustrates minimal example which is storing an
integer value and then dumping trace into a buffer:

~~~~~~~~~~
1. int32_t value = 123;
2. uint8_t buffer[1000];
3. VarTrace<> trace;
4. trace.Log(kInfoLevel, 1, value);
5. std::size_t dumped_size = trace.DumpInto(buffer, 1000);
~~~~~~~~~~

Line 3 creates a trace with default parameters that is it has size
4kB, is split into 8 blocks, accepts messages of all log levels and
does not use lock. The default constructor installs simple counter as
timestamp function. Thus `value` on line 4 will be save with timestamp
0, message id 1 and type id 5 (type id for `int32_t` is 5). The trace
is serialized into a buffer on line 5. The dump function returns the
size of written data. If the trace contains more data then the buffer
size, which is passed as the second argument, then only the older
messages will be serialized.

The behavior of a trace object can be changed by passing template and
regular arguments to VarTrace [constructor](\ref vartrace::VarTrace).
For example the statement

~~~~~~~~~~
VarTrace<ErrorLogLevel> trace(4096, 4, buffer);
~~~~~~~~~~

creates a trace object that stores messages at least error level
severity in an allocated `buffer` of size 4096. The buffer is split
into 4 parts so around 3kB will be serialized by function
vartrace::VarTrace::DumpInto().

User defined types can be stored either by memory copy or by calling a
custom function which can store data by calling vartrace log function.
To force the library to use such behaviour a user must set copy trait.

If it is possible to add member to a class then member function can be
used to store it in a trace:

~~~~~~~~~~
// some header

namespace test {
class SelfLogClass {
 public:
  int ivar;
  double dvar;
  double dont_log_array[10];

  // Function that chooses which members to store.
  template<class LoggerPointer>
  void LogItself(LoggerPointer trace) const {
    trace->Log(kInfoLevel, 101, ivar);
    trace->Log(kInfoLevel, 102, dvar);
  }
};
}

// Register SelfLogClass, the statement must be outside all namespaces.
VARTRACE_SET_SELFLOGGING(test::SelfLogClass);

// some source

test::SelfLogClass obj;
obj.ivar = 1234;
obj.dvar = 12e-34;

trace.Log(kInfoLevel, 1, obj);
~~~~~~~~~~

In this example a class defines a function with name `LogItself` which
takes a pointer to a trace object. It is up to a class creator how to
store the class in a trace. For example, subtrace can be used to speed
up logging. VarTrace objects with different log threshold have
different types so it is better to use template for `LogItself`
function.

It is also possible to use non member function for logging:

~~~~~~~~~~
// some header

namespace test {
struct CustomLogStruct {
  int ivar;
  double dvar;
  double dont_log_array[10];
};
}

namespace vartrace {
// Custom logging function.
template<class LoggerPointer>
void LogObject(const test::CustomLogStruct &object, LoggerPointer trace) {
  trace->Log(kInfoLevel, 101, object.ivar);
  trace->Log(kInfoLevel, 102, object.dvar);
}
}  // namespace vartrace

// the statement must be outside all namespaces
VARTRACE_SET_LOG_FUNCTION(test::CustomLogStruct);

// some source

test::CustomLogStruct obj;
obj.ivar = 1234;
obj.dvar = 12e-34;

trace.Log(kInfoLevel, 1, obj);
~~~~~~~~~~

This is a good alternative to member function in the case when a user
does not have access to a class definition.

## Building

To build the library one has to compile files from `src/vartrace` and
copy `include/vartrace` into an include directory. Alternatively the
library can be build using cmake.

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


