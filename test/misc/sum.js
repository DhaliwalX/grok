function sum(a, b) {
    var result = 0;
    var i = 1;
    var max = 32;
    var c = 0;

    var sum = 0;
    while (max--) {
        sum = ((a & i) ^ (b & i) ^ c);
        c = (a & b & i) | (((a & i) ^ (b & i)) & c);
        i = i << 1;
        result = result | sum;
    }

    return result;
}

console.log(sum(1, 2));

