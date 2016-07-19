function TestNestedLoops() {
  var sum = 0;
  for (i = 0; i < 20; i = i + 1) {
    for (j = 0; j < 20; j = j + 1) {
      sum = sum + 1;
    }
  }
  return sum;
}
assert_equal(20 * 20, TestNestedLoops());
