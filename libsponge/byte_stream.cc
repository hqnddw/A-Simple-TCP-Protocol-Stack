#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`
#include <iostream>
using namespace std;

ByteStream::ByteStream(const size_t capacity) : _buffer(""), _capacity(capacity), _read(0), _write(0) {}

size_t ByteStream::write(const string &data) {
    if (remaining_capacity() == 0) {
        return 0;
    }
    size_t write_len;
    if (data.size() < remaining_capacity()) {
        write_len = data.size();
    } else {
        write_len = remaining_capacity();
    }
    string tmp = data.substr(0, write_len);
    _buffer.append(tmp);
    _write += write_len;

    return write_len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    if (_buffer.size() <= len) {
        return _buffer;
    }
    return _buffer.substr(0, len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    int t = min(len, _buffer.size());
    if (_buffer.size() <= len) {
        _buffer.clear();
    } else {
        _buffer = _buffer.substr(len);
    }
    _read += t;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string res = peek_output(len);
    pop_output(len);
    return res;
}

void ByteStream::end_input() { set_error(); }

bool ByteStream::input_ended() const { return _error; }

size_t ByteStream::buffer_size() const { return _buffer.size(); }

bool ByteStream::buffer_empty() const { return _buffer.empty(); }

bool ByteStream::eof() const { return input_ended() && buffer_empty(); }

size_t ByteStream::bytes_written() const { return _write; }

size_t ByteStream::bytes_read() const { return _read; }

size_t ByteStream::remaining_capacity() const { return _capacity - _buffer.size(); }
