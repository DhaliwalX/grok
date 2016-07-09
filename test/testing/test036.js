function fib(n) {
  var f0 = 0, f1 = 1;
  for (0; n > 0; n = n -1) {
    var f2 = f0 + f1;
    f0 = f1; f1 = f2;
  }
  return f0;
}

assert_equal(2111485077978050, fib(75));
