///////////////////////////////////////////////////////////////////////////////
// disks.hpp
//
// Definitions for two algorithms that each solve the 
// alternating disks problem.
//
// As provided, this header has four functions marked with 
// TODO comments.
// You need to write in your own implementation of these 
// functions.
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// State of one disk, either light or dark.
enum disk_color { DISK_LIGHT, DISK_DARK };

// Data structure for the state of one row of disks.
class disk_state {
private:
  std::vector<disk_color> _colors;

public:

  disk_state(size_t light_count)
    : _colors(light_count * 2, DISK_LIGHT) {

      assert(dark_count() > 0);

      for (size_t i = 1; i < _colors.size(); i += 2) {
        _colors[i] = DISK_DARK;
      }
  }

  // Equality operator for unit tests.
  bool operator== (const disk_state& rhs) const {
    return std::equal(_colors.begin(), _colors.end(), rhs._colors.begin());
  }

  size_t total_count() const {
    return _colors.size();
  }

  size_t dark_count() const {
    return total_count() / 2;
  }

  size_t light_count() const {
    return dark_count();
  }

  bool is_index(size_t i) const {
    return (i < total_count());
  }

  disk_color get(size_t index) const {
    assert(is_index(index));
    return _colors[index];
  }

  void swap(size_t left_index) {
    assert(is_index(left_index));
    auto right_index = left_index + 1;
    assert(is_index(right_index));
    std::swap(_colors[left_index], _colors[right_index]);
  }

  std::string to_string() const {
    std::stringstream ss;
    bool first = true;
    for (auto color : _colors) {
      if (!first) {
        ss << " ";
      }

      if (color == DISK_LIGHT) {
        ss << "L";
      } else {
        ss << "D";
      }

      first = false;
    }
    return ss.str();
  }

  // Return true when this disk_state is in alternating format. That means
  // that the first disk at index 0 is light, the second disk at index 1
  // is dark, and so on for the entire row of disks.
  bool is_alternating() const {
    //number of total disks
    auto size = total_count();

    //iterate over each disk pairs from start to end
    for (size_t i = 0; i < size - 1; i += 2) {
      //check if light and dark disks are interleaving by check if current is light and next is dark
      if (get(i) != DISK_LIGHT || get(i + 1) != DISK_DARK) return false;
    }

    return true;
  }

  // Return true when this disk_state is fully sorted, with all light disks
  // on the left (low indices) and all dark disks on the right (high
  // indices).
  bool is_sorted() const {
    //number of total disks
    auto lightSize = light_count();

    //iterate over the disks to check if they are sorted by color
    for (size_t i = 0; i < lightSize; i++) {
      //check if current is light
      if (get(i) != DISK_LIGHT) return false;
      //check if the next nth disk is dark
      if (get(i + lightSize) != DISK_DARK) return false;
    }

    return true;
  }
};

// Data structure for the output of the alternating disks problem. That
// includes both the final disk_state, as well as a count of the number
// of swaps performed.
class sorted_disks {
private:
  disk_state _after;
  unsigned _swap_count;

public:

  sorted_disks(const disk_state& after, unsigned swap_count)
    : _after(after), _swap_count(swap_count) { }

  sorted_disks(disk_state&& after, unsigned swap_count)
    : _after(after), _swap_count(swap_count) { }

  const disk_state& after() const {
    return _after;
  }

  unsigned swap_count() const {
    return _swap_count;
  }
};

// Algorithm that sorts disks using the left-to-right algorithm.
sorted_disks sort_left_to_right(const disk_state& before) {

  // check that the input is in alternating format  
  assert(before.is_alternating());
  
  //total number of swaps made
  int swapCount = 0;
  //number of disks of each color and total number of disks
  auto lightCount = before.light_count(), totalCount = before.total_count();

  //iterate n times from left to right
  for (size_t i = 0; i < lightCount; i++) {
    //each left to right scanning, skip some of the disk in head and tail we know already sorted
    for (auto j = i + 1; j < totalCount - i - 1; j++) {
      //make swaps when necessary
      if (before.get(j) == DISK_DARK && before.get(j + 1) == DISK_LIGHT) {
        //do the swap
        const_cast<disk_state&>(before).swap(j);
        //update swap count
        swapCount++;
      }
    }
  }

  return sorted_disks(before, swapCount);
}

// Algorithm that sorts disks using the lawnmower algorithm.
sorted_disks sort_lawnmower(const disk_state& before) {
  // check that the input is in alternating format
  assert(before.is_alternating());

  //total number of swaps made
  int swapCount = 0;
  //number of disks of each color and total number of disks
  auto lightCount = before.light_count(), totalCount = before.total_count();

  //iterate n / 2 times from the lawnmower move
  for (size_t i = 0; i < lightCount / 2; i++) {
    //move from left to right, skip some of the disk in head and tail we know already sorted
    for (auto j = i + 1; j < totalCount - i; j++) {
      if (before.get(j) == DISK_DARK && before.get(j + 1) == DISK_LIGHT) {
        //do the swap
        const_cast<disk_state&>(before).swap(j);
        //update swap count
        swapCount++;
    }

    //update i as we know after a left to right move, there is one more disk we know that is soreted in 
    //both head and tail side
    i++;

    //move from right to left, skip some of the disk in head and tail we know already sorted
    for (auto j = totalCount - i - 1; j > i; j--) {
        if (before.get(j) == DISK_LIGHT && before.get(j - 1) == DISK_DARK) {
          //do the swap
          const_cast<disk_state&>(before).swap(j - 1);
          //update swap count
          swapCount++;
        }
      }
    }
  }

  return sorted_disks(before, swapCount);
}
