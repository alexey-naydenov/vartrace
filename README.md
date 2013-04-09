# Vartrace

C++ library for storing data at runtime. Essentially this library is a
logger that stores values in binary format along with timestamp in a
circular buffer. The design is base on
[policies](http://en.wikipedia.org/wiki/Policy-based_design) and
traits.

## Testing

The project uses google test suite. To setup testing (testing does
not work due to addition of log level functionality):

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

3. Create `biuld` directory and test:

    ```sh
    mkdir build && cd build
    cmake .. && make vartrace
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
following message then it means that overflow happenned and time
stamp of the consequent messages should be incremented by the max
value of the time stamp column.

### Features

- Horizontal axis can either be time stamp, counter of points for
some data, value of some data set.
- Axis can have either linear or logarithmic scale.
- Different data messages can have different scale factors.
- Array values can be displayed as a verticaly arranged set of
dots at the corresponding time stamp value.
- Array can be reduced to a single value, for example with
functions: index, min, max, sum, avg, median.
- Multiple values per array can be displayed on a single plot.
- Data can be divided into frames (timestamp is used) based on
some condition.
- User has a choice to display all data, data for some frames or
one point per frame.
- Data for single frame can be aggregated, for example by
funcions: min, max, sum, avg, median.
- Frames can be filtered based on some condition.
- Data points can be fileterd based on their value or last value
of some other data set.
