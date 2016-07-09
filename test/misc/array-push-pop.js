a = [];

function test(a) {
    for (i = 0; i < 999; i++) {
        a.push(i);
    }
    return a;
}

res = test(a);
console.log(a);
console.log(res);

