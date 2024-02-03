/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file dbg.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>

#include "macrologger.h"

namespace hmap
{

class Recorder
{
public:
  Recorder(std::string name) : name(name)
  {
  }

  void dump()
  {
    std::cout << std::setw(20) << this->name;
    std::cout << std::setw(10) << this->nb_calls;
    std::cout << std::setw(20) << this->total / (float)this->nb_calls << " ms";
    std::cout << std::endl;
  }

  void start()
  {
    this->t0 = std::chrono::high_resolution_clock::now();
    this->nb_calls++;
  }

  void stop()
  {
    std::chrono::high_resolution_clock::time_point t1 =
        std::chrono::high_resolution_clock::now();
    this->total += (float)std::chrono::duration_cast<std::chrono::nanoseconds>(
                       t1 - t0)
                       .count() *
                   1e-6f;
  }

private:
  std::string                                    name;
  int                                            nb_calls = 0;
  std::chrono::high_resolution_clock::time_point t0;
  float                                          total = 0.f;
};

class Timer
{
public:
  Timer(std::string sid = "") : sid(sid){};
  ~Timer()
  {
    this->dump();
  };

  void dump()
  {
    std::cout << "Timer dump: " << this->sid << std::endl;
    for (auto &n : records)
      n.second->dump();
  }

  void start(std::string name)
  {
    if (records.find(name) == records.end())
    {
      Recorder new_recorder = Recorder(name);
      data.push_back(new_recorder);
      records[name] = &data.back();
    }
    records[name]->start();
  }

  void stop(std::string name)
  {
    if (records.find(name) != records.end())
      records[name]->stop();
    else
      std::cout << "Warning! Trying to stop an unknown timer: " << name
                << std::endl;
  }

private:
  std::string                       sid;
  std::map<std::string, Recorder *> records;
  std::list<Recorder>               data;
};

} // namespace hmap
