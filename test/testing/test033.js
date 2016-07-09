function TestNestedLoops() {
  var sum = 0;
  for (i = 0; i < 200; i = i + 1) {
    for (j = 0; j < 200; j = j + 1) {
      sum = sum + 1;
    }
  }
  return sum;
}
assert_equal(200 * 200, TestNestedLoops());
