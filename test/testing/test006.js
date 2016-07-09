// Test compilation of loops.

var n = 1;
for (i = 1; (6 - i); i++) {
  // Factorial!
  n = n * i;
}
assert_equal(120, n);

// Test assignments in the loop condition.
function f(i, n) {
  while((n = n - 1) > 0) {
    i = n + 1;
  }
  return i;
}
assert_equal(1, f(0, 42));
