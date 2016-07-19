// array reverse
var myArray = ['one', 'two', 'three'];
myArray.reverse();
// ['three', 'two', 'one']
assert_equal(myArray[0], 'three');
assert_equal(myArray[1], 'two');
assert_equal(myArray[2], 'one');

var arr = new Array(100);

for (i = 0; i < 100; i++) {
    arr[i] = i;
}
arr.reverse();
for (i = 99; i >= 0; i--) {
    assert_equal(arr[i], 99 - i);
}
