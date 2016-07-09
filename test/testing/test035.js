function TestCreateString(n)
{
  var l = n * 1;
  var r = 'r';
  while (r.length < n)
  {
    r = r + r;
  }
  return r;
}

assert_equal("r", TestCreateString(1));
assert_equal("rr", TestCreateString(2));
assert_equal("rrrr", TestCreateString(3));
assert_equal("rrrrrrrr", TestCreateString(6));
