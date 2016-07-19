var sports = ['soccer', 'baseball'];
var total = sports.push('football', 'swimming');

assert_equal(total, 4);
assert_equal(sports[0], 'soccer'); 
assert_equal(sports[1], 'baseball'); 
assert_equal(sports[2], 'football'); 
assert_equal(sports[3], 'swimming'); 

var myFish = ['angel', 'clown', 'mandarin', 'sturgeon'];

var popped = myFish.pop();

assert_equal(myFish.length, 3);
assert_equal(popped, 'sturgeon');

var myFish = ['angel', 'clown', 'mandarin', 'surgeon'];

var shifted = myFish.shift();

assert_equal(myFish[myFish.length - 1], 'surgeon');
assert_equal(shifted, 'angel');

var arr = [1, 2];

var r = arr.unshift(0); // result of call is 3, the new array length
// arr is [0, 1, 2]
assert_equal(arr.length, r);
assert_equal(arr[0], 0);
assert_equal(arr[2], 2);

r = arr.unshift(-2, -1); // = 5
// arr is [-2, -1, 0, 1, 2]
assert_equal(arr.length, r);
assert_equal(arr[0], -2);
assert_equal(arr[1], -1);
