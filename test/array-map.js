a = [1,3,6];

function Mapper(a, i) { if (a < 5) return a; }

res = a.map(Mapper);

console.log(res);