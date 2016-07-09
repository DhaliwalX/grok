function f() {
  return (42 + (0 == 1 ? 1 : 2));
}


function g(x) {
  return (x + (0 == 1 ? 1 : 2));
}


function h(x) {
  return ((x + 1) + (0 == 1 ? 1 : 2));
}

assert_equal(44, f());
assert_equal(45, g(43));
assert_equal(47, h(44));