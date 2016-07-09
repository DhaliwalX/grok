function f(a,b) {
  return (b < a) - (a < b);
}

assert_equal(0, f(0,0));
assert_equal(1, f(1,0));
assert_equal(-1, f(0,1));
