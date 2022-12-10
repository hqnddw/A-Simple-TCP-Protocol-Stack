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
            bool flag = true;
            auto it = _stream_assembler.begin();
            while (it != _stream_assembler.end()) {
                if (cur_index < it->first) {
                    if (cur_index + input.size() < it->first) {
                        cout << "--------enter if if 1---------" << endl;
                        break;
                    } else if (cur_index + input.size() == it->first) {
                        cout << "--------enter if if 2---------" << endl;
                        // 合并后进入下一次循环
                        input.append(it->second.data);
                        cur_eof |= it->second.eof;
                        it = _stream_assembler.erase(it);
                    } else if (cur_index + input.size() > it->first &&
                               cur_index + input.size() <= it->first + it->second.data.size()) {
                        cout << "--------enter if if 3---------" << endl;
                        input = input.substr(0, it->first - cur_index);
                        input.append(it->second.data);
                        cur_eof |= it->second.eof;
                        it = _stream_assembler.erase(it);
                    } else if (cur_index + input.size() > it->first + it->second.data.size()) {
                        cout << "--------enter if if 4---------" << endl;
                        cur_eof |= it->second.eof;
                        it = _stream_assembler.erase(it);
                    }
                } else if (cur_index >= it->first && cur_index <= it->first + it->second.data.size()) {
                    if (cur_index + input.size() <= it->first + it->second.data.size()) {
                        flag = false;
                        cout << "--------enter if if 5---------" << endl;
                        break;
                    } else if (cur_index + input.size() > it->first + it->second.data.size()) {
                        cout << "--------enter if if 6---------" << endl;
                        string tmp = input.substr(it->first + it->second.data.size() - cur_index);
                        input = it->second.data.append(tmp);
                        cur_index = it->first;
                        cur_eof |= it->second.eof;
                        it = _stream_assembler.erase(it);
                    }
                } else {
                    cout << "--------enter if if 7---------" << endl;
                    it++;
                }
            }
            if (flag) {
                _stream_assembler[cur_index] = DataSegment{input, cur_eof};
                _unassemble_size += input.size();
            }
        }
    } else {  // 目标数据（cur_index == _next_index）
        if (_stream_assembler.empty()) {
            _output.write(input);
            _next_index += input.size();
        } else {
            bool flag = true;
            auto it = _stream_assembler.begin();
            while (it != _stream_assembler.end()) {
                if (cur_index < it->first) {
                    if (cur_index + input.size() < it->first) {
                        cout << "--------enter if 1---------" << endl;
                        break;
                    } else if (cur_index + input.size() == it->first) {
                        cout << "--------enter if 2---------" << endl;
                        // 合并后进入下一次循环
                        flag = true;
                        input.append(it->second.data);
                        cur_eof |= it->second.eof;
                        it = _stream_assembler.erase(it);
                    } else if (cur_index + input.size() > it->first &&
                               cur_index + input.size() <= it->first + it->second.data.size()) {
                        cout << "--------enter if 3---------" << endl;
                        input = input.substr(0, it->first - cur_index);
                        input.append(it->second.data);
                        cur_eof |= it->second.eof;
                        it = _stream_assembler.erase(it);
                    } else if (cur_index + input.size() > it->first + it->second.data.size()) {
                        cout << "--------enter if 4---------" << endl;
                        cur_eof |= it->second.eof;
                        it = _stream_assembler.erase(it);
                    }
                } else if (cur_index >= it->first && cur_index <= it->first + it->second.data.size()) {
                    if (cur_index + input.size() <= it->first + it->second.data.size()) {
                        flag = false;
                        cout << "--------enter if 5---------" << endl;
                        break;
                    } else if (cur_index + input.size() > it->first + it->second.data.size()) {
                        cout << "--------enter if 6---------" << endl;
                        string tmp = input.substr(it->first + it->second.data.size() - cur_index);
                        input = it->second.data.append(tmp);
                        cur_index = it->first;
                        cur_eof |= it->second.eof;
                        it = _stream_assembler.erase(it);
                    }
                } else {
                    cout << "--------enter if 7---------" << endl;
                    it++;
                }
            }
            if (flag) {
                _output.write(input);
                _next_index += input.size();
            }
        }
    }

    if (cur_eof && _stream_assembler.empty()) {
        // cout << "------------set_error------------" << endl;
        _output.set_error();
    }

    return;
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t unassemble_size = 0;
    for (auto it : _stream_assembler) {
        unassemble_size += it.second.data.size();
    }
    return unassemble_size;
}

bool StreamReassembler::empty() const { return _stream_assembler.empty(); }
