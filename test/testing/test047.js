var arr = new Array(100);

for (i = 0; i < 100; i++)
{
    arr[i] = i << 0;
}

var res = arr.slice(50);
for (i = 50; i < 100; i++) {
    assert_equal(res[i - 50], i);
}

var start = 5;
var end = 95;
res = arr.slice(5, 95);
for (i = (start); i < end; i++) {
    assert_equal(res[i - start], i);
}
