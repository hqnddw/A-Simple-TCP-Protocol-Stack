#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`
#include <iostream>

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), _stream_assembler(), _next_index(0), _unassemble_size(0) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // 空字符串直接返回
    if (data.empty()) {
        if (eof) {
            _output.set_error();
        }
        return;
    }

    string input = data;
    size_t cur_index = index;
    bool cur_eof = eof;
    // 去除掉overlapping部分
    if (_next_index > index) {
        // data完全重复
        if (cur_index + data.size() <= _next_index) {
            return;
        }
        input = input.substr(_next_index - cur_index);
        cur_index = _next_index;
    }
    // 去掉超过capacity的部分
    if (cur_index - _output.bytes_read() + input.size() > _capacity) {
        input = input.substr(0, _capacity - cur_index + _output.bytes_read());
        cur_eof = false;
    }

    // 后面的数据提前到达，放入unassembler中
    if (cur_index > _next_index) {
        if (_stream_assembler.empty()) {
            cout << "_stream_assembler empty put" << endl;
            _stream_assembler[cur_index] = DataSegment{input, cur_eof};
            _unassemble_size += input.size();
        } else {
            if (merge_segment(input, cur_index, cur_eof)) {
                _stream_assembler[cur_index] = DataSegment{input, cur_eof};
                _unassemble_size += input.size();
            }
        }
    } else {  // 目标数据（cur_index == _next_index）
        if (_stream_assembler.empty()) {
            _output.write(input);
            _next_index += input.size();
        } else {
            if (merge_segment(input, cur_index, cur_eof)) {
                _output.write(input);
                _next_index += input.size();
            }
        }
    }

    if (cur_eof && _stream_assembler.empty()) {
        _output.set_error();
    }

    return;
}

// 合并input和stream_assembler中的数据段
bool StreamReassembler::merge_segment(string &input, size_t &cur_index, bool &cur_eof) {
    bool flag = true;
    auto it = _stream_assembler.begin();
    while (it != _stream_assembler.end()) {
        /*
                        |-----it-----|
        |---input---|
        |------input----|
        |----------input---------|
         */
        if (cur_index < it->first) {
            if (cur_index + input.size() < it->first) {
                break;
            } else if (cur_index + input.size() == it->first) {
                // 合并后进入下一次循环
                flag = true;
                input.append(it->second.data);
                cur_eof |= it->second.eof;
                it = _stream_assembler.erase(it);
            } else if (cur_index + input.size() > it->first &&
                       cur_index + input.size() <= it->first + it->second.data.size()) {
                input = input.substr(0, it->first - cur_index);
                input.append(it->second.data);
                cur_eof |= it->second.eof;
                it = _stream_assembler.erase(it);
            } else if (cur_index + input.size() > it->first + it->second.data.size()) {
                cur_eof |= it->second.eof;
                it = _stream_assembler.erase(it);
            }
            /*
                |---------it-------|
                    |---input---|
                    |------input------|
            */
        } else if (cur_index >= it->first && cur_index <= it->first + it->second.data.size()) {
            if (cur_index + input.size() <= it->first + it->second.data.size()) {
                flag = false;
                break;
            } else if (cur_index + input.size() > it->first + it->second.data.size()) {
                string tmp = input.substr(it->first + it->second.data.size() - cur_index);
                input = it->second.data.append(tmp);
                cur_index = it->first;
                cur_eof |= it->second.eof;
                it = _stream_assembler.erase(it);
            }
            /*
            |------it-----|
                            |---input---|
            */
        } else {
            it++;
        }
    }
    return flag;
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t unassemble_size = 0;
    for (auto it : _stream_assembler) {
        unassemble_size += it.second.data.size();
    }
    return unassemble_size;
}

bool StreamReassembler::empty() const { return _stream_assembler.empty(); }
