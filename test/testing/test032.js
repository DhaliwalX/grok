function f1(a,b,c) {
  var x = 0;
  var y = 0;
  var z = 0;
  while (a > 0) { x = x + 19; a--; }
  while (b > 0) { y = y + 23; b--; }
  while (c > 0) { z = z + 29; c--; }
  return x + y + z;
}

function f2(a,b,c) {
  var x = 0;
  var y = 0;
  var z = 0;
  while (a > 0) { x = x + 19; a--; }
  while (b > 0) { y = y + 23; b--; }
  while (c > 0) { z = z + 29; c--; }
  return x + y + z;
}


function f3(a,b,c) {
  var x = 0;
  var y = 0;
  var z = 0;
  while (a > 0) { x = x + 19; a--; }
  while (b > 0) { y = y + 23; b--; }
  while (c > 0) { z = z + 29; c--; }
  return x + y + z;
}

function check(f,a,b,c) {
  assert_equal(a * 19 + b * 23 + c * 29, f(a,b,c));
}

check(f1, 50000,     5,     6);
check(f2,     4, 50000,     6);
check(f3,    11,    12, 50000);
