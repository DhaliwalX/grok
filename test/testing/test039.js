function ret(a)
{
    return a;
}

function add(a, b)
{
    return ret(a) + ret(b);
}

var result = add(10, 20);
assert_equal(result, 30);
result = add.apply(null, [10, 20]);
assert_equal(result, 30);
result = add.call(null, 30, 20);
assert_equal(result, 50);

var obj = {
    a : 10,
    b : 20
};
x = 12;
y = 13;

function r()
{
    return this.x + this.y;
}

function addthis()
{
    assert_equal(r(), 25);
    return this.a + this.b;
}

result = addthis.call(obj);
assert_equal(result, 30);
result = addthis.apply(obj);
assert_equal(result, 30);
