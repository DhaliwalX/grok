
///////////////////////////////////////////////////////////////////////////////
//
//    +-----------------------------------------------------------+
//    |          Grok : A Naive JavaScript Interpreter            |
//    +-----------------------------------------------------------+
//
// Copyright 2015 Pushpinder Singh
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
//  \file:  main.cc
//  \description:  definition of main
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#include "../src/grok.h"
#include "../test/test.h"

int main(int argc, char *argv[]) {
 // test::testmain();
  Grok grok;
  grok.Run(argc, argv);
  return 0;
}
