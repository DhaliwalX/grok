a = 3;

function A(a) { return a; }

function map(a, f) { res = f(a); res = f(a); res = f(a); return res; }

map(a, A);