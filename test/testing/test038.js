function ret(a)
{
    assert_equal(a, a);
    return a;
}

function sq(a)
{
    return ret(a) * ret(a);
}

function cube(a)
{
    return ret(a) * sq(a);
}

var arr = [1,2,3];

var result = arr.map(sq);
assert_equal(result[0], 1);
assert_equal(result[1], 4);
assert_equal(result[2], 9);

result = arr.map(cube);

assert_equal(result[0], 1);
assert_equal(result[1], 8);
assert_equal(result[2], 27);
