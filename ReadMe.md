[![Build Status](https://travis-ci.org/PrinceDhaliwal/grok.svg?branch=master)](https://travis-ci.org/PrinceDhaliwal/grok)
## Grok - Another JavaScript interpreter

_Grok_ is a simple and tiny javascript interpreter written in C++. I made it because I wanted to learn how Compiler works.

### Features

Declare variables:

```js
var a = 10;

// or create strings
var b = 'grok';

// or create an array
var c = [ 1,2,4,6 ];

// or create objects
var d = {
  a : 100,
  b : {
    arr : [1,2,4,5]
  }
};

```
Write complex expressions like:

```js
var sum = d.a + c[0] + c.length + d.b.arr[2];

```

Create functions:

```js
function fib(n) {
    if (n == 0 || n == 1)
        return 0;
    else if (n == 2)
        return 1;

    return fib(n - 1) + fib(n - 2);
}

// call functions
console.log(fib(10)); // yup console object is supported
```

Call Array methods:

```js
var arr = [ 1, 2, 3, 4, 5 ];
var squares = arr.map(function(element) {
            return element * element;
}); // squares == [ 1, 4, 9, 16, 25 ]
```

## Building
### Requirements
+ Boost C++ Library `sudo apt-get install libboost-all-dev`
+ GNU Readline Library `sudo apt-get install libreadline6-dev`
+ pthread
+ cmake `sudo apt-get install cmake`
+ C++ compiler with C++14 support `sudo apt-get install g++-5`
+ Ubuntu (not tested for other operating systems)

### Compiling
One step: `cd grok && mkdir build && cd build && cmake .. && make -j4`

### Contribute
Grok is currently in development stage. New features will be added slowly.
It is just a free time project. I work on it whenever I am free. If you want
to help, feel free to open pull requests or create issues. Please do __star__
the project if you like it.

### Things that do not work

+ switch case statements
+ break, continue statements
+ `this` may not work properly
+ constructors
