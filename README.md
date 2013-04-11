# Vartrace

C++ library for storing data at runtime. Essentially this library is a
logger that stores values in binary format along with timestamp in a
circular buffer. The design is base on
[policies](http://en.wikipedia.org/wiki/Policy-based_design) and
traits.

Goals:

* Compact data. Values are stored in binary form with minimal
  additional information. Each record contains up to 8 bytes of service
  data (4 byte timestamp, record type, data type and data
  size). Messages can be bundled together so that timestamp stored
  only once.

* Speed. As much as possible is done during compile time. Log function
  dispatches on type and log level during compilation (one cannot
  change log level at runtime). Copying of simple types is done
  through assignment which gives small increase for int types (20% or
  so). Unless class defines log function it will be copied through
  `memcpy`.

* Modularity. The behavior of log objects can be tuned through
  policies. There are 4 policies: log level, object creation (for
  example: singleton or one per create call), locking and data storage
  creation.

* Simple syntax. In basic form a variable value can be stored like
  `logger->Log(kInfoLevel, value);` where `value` can be pretty much
  anything.

* Portable. In theory the library can be compiled by any C++ compiler
  with good template support that has STL implementation. Vartrace
  uses `<boost/shared_ptr.hpp>` and `<boost/shared_array.hpp>` which
  can be used without much of the rest of boost.

## Testing

The project uses Google test suite. To setup testing:

1. Download and unpack `gtest` into some directory:

    ```sh
    cd ~/tmp
    wget http://googletest.googlecode.com/files/gtest-1.6.0.zip 
    unzip gtest-1.6.0.zip
    ```

2. In `vartrace` create `gtest_build` folder and build `gtest`:

    ```sh
    mkdir gtest_build && cd gtest_build
    cmake ~/tmp/gtest-1.6.0 && make && cd ..
    ```

3. Create `build` directory and test:

    ```sh
    mkdir build && cd build
    cmake .. && make vartrace_test && make profile
	ctest
    ```

## Profiling

Profiling can be done either through gtests of by launching files for
each data type separately. First compile in release:

```sh
cmake -DCMAKE_BUILD_TYPE=Release ..
make vartrace_test && make profile
```

Then

```sh
ctest
```

or

```sh
time ./trunk/tests/profile_double
```

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

### Features

- Horizontal axis can either be time stamp, counter of points for
some data, value of some data set.
- Axis can have either linear or logarithmic scale.
- Different data messages can have different scale factors.
- Array values can be displayed as a vertically arranged set of
dots at the corresponding time stamp value.
- Array can be reduced to a single value, for example with
functions: index, min, max, sum, avg, median.
- Multiple values per array can be displayed on a single plot.
- Data can be divided into frames (timestamp is used) based on
some condition.
- User has a choice to display all data, data for some frames or
one point per frame.
- Data for single frame can be aggregated, for example by
functions: min, max, sum, avg, median.
- Frames can be filtered based on some condition.
- Data points can be filtered based on their value or last value
of some other data set.
