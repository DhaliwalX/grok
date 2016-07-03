function add(a, b) {
    return a + b;
}
function sub(a,b) {
    return a - b;
}

function double(a,b,c) {
    return add(a,b) + sub(b,c);
}

result = double(10, 20, 30);

console.log(result == 20);

