// simple loop testing
var m = 0;
for (i = 0; i < 1000; i++) {
    m++;
}
assert_equal(m, 1000);

m = 0;
while (m < 1000) {
    m++;
}

assert_equal(m, 1000);

m = 0;
do {
    m++;
} while (m < 1000);

var n = 1;
for (i = 1; (6 - i); i++) {
  // Factorial!
  n = n * i;
}
assert_equal(120, n);

// this test fails
// // Test assignments in the loop condition.
// function f(i, n) {
//   while((n = n - 1) > 0) {
//     i = n + 1;
//   }
//   return i;
// }
// assert_equal(1, f(0, 42));

