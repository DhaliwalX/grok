///////////////////////////////////////////////////////////////////////////////
//
//    +-----------------------------------------------------------+
//    |          Grok : A Naive JavaScript Interpreter            |
//    +-----------------------------------------------------------+
//
// Copyright 2015 Prince Dhaliwal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//
//  \file:  timer.h
//  \description:  implementation of Timer class useful for debugging purposes
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MEM_ALLOC_H_
#define MEM_ALLOC_H_

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <ctime>
#include <iostream>
#include <limits>
#include <random>

// useful for debug operations
class Timer {
public:

  static const int64_t kMilliSecondsPerSec = 1000;
  static const int64_t kMilliSecondsPerMinutes = kMilliSecondsPerSec * 60;
  static const int64_t kMilliSecondsPerHours = kMilliSecondsPerMinutes * 60;
  static const int64_t kMilliSecondsPerDay = kMilliSecondsPerHours * 24;
  static const int64_t kSecondsPerMinute = 60;
  static const int64_t kSecondsPerHour = 3600;
  static const int64_t kSecondsPerDay = 3600 * 24;


  typedef std::chrono::microseconds            MicroSeconds;
  typedef std::chrono::milliseconds            MilliSeconds;
  typedef std::chrono::nanoseconds             NanoSeconds;
  typedef std::chrono::high_resolution_clock   HighResolutionClock;
  typedef std::chrono::system_clock            SystemClock;
  typedef std::chrono::hours                   Hours;
  typedef std::chrono::seconds                 Seconds;

  void Start() {
    start_ = std::chrono::system_clock::now();
  }

  void StartHR() {
    this->HighResStart_ = std::chrono::high_resolution_clock::now();
  }


  double SecondsPassed() {
    std::chrono::time_point<std::chrono::system_clock> mid;
    mid = std::chrono::system_clock::now();
    std::chrono::duration<long double> elapsedMilliSeconds =
      mid - start_;
    return elapsedMilliSeconds.count();
  }

  long long MilliSecondsPassed() {
    return std::chrono::duration_cast<MilliSeconds>
      (std::chrono::system_clock::now() - start_).count();
  }

  long long MicrosecondsPassed() {
    return static_cast<long long>(std::chrono::duration_cast<MicroSeconds>
      (std::chrono::system_clock::now() - this->start_).count());
  }

  long long NanoSecondsPassed() {
    return static_cast<long long>(std::chrono::duration_cast<NanoSeconds>
      (std::chrono::high_resolution_clock::now() - this->HighResEnd_).count());
  }

  void Stop() {
    this->end_ = std::chrono::system_clock::now();
  }

  void StopHR() {
    this->HighResEnd_ = std::chrono::high_resolution_clock::now();
  }

  long long ElapsedTimeInMicroseconds() {
    return std::chrono::duration_cast<MicroSeconds>
      (this->end_ - this->start_).count();
  }

  long long ElapsedTimeInMilliSeconds() {
    return std::chrono::duration_cast<MilliSeconds>
      (this->end_ - this->start_).count();
  }

  long long ElapsedTimeInNanoSeconds() {
    return std::chrono::duration_cast<NanoSeconds>
      (this->HighResEnd_ - this->HighResStart_).count();
  }

  void PrintNS() {
    std::cout << "Time Elapsed: "
      << ElapsedTimeInNanoSeconds() << "ns" << std::endl;
  }

  void PrintMS() {
    std::cout << "Time Elapsed: "
      << ElapsedTimeInMilliSeconds() << "ms" << std::endl;
  }

  void PrintUS() {
    std::cout << "Time Elapsed: "
      << ElapsedTimeInMicroseconds() << "us" << std::endl;
  }

  long long ElapsedTime() {

  }

private:
  std::chrono::time_point<std::chrono::system_clock>            start_;
  std::chrono::time_point<std::chrono::system_clock>            end_;
  std::chrono::time_point<std::chrono::high_resolution_clock>   HighResStart_;
  std::chrono::time_point<std::chrono::high_resolution_clock>   HighResEnd_;
};  // Timer


class Factory {
public:
  static void * Create(size_t size) {
    return std::malloc(size);
  }

  static void Destroy(void *object) {
    if (object != nullptr)
      std::free(object);
  }

  static int RandomNumber(int start, int end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(start, end);
    return dist(gen);
    return start + std::rand() % (end - start);
  }

  static std::time_t Time() {
    return std::clock();
  }
};


#endif
