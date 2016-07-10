// conditional statements
a = 10;
if (true)
    a = 20;
assert_equal(a, 20);

if (false)
    a = 124;
assert_equal(a, 20);

if (a > 10)
    a = 10;
else
    a = 20;

assert_equal(a, 10);

if (a > 10)
    a = 20;
else 
    a = 14;

assert_equal(a, 14);

if (a > 15)
    a = 2;
else if (a < 20)
    a = 5;

assert_equal(a, 5);

if (false)
    a = 1;
else if (false)
    a = 2;
else if (true)
    a = 34;
else if (false)
    a = 240;
else if (false)
    a = 20;
else if (true)
    a = 5;
else if (false)
    a = 50;
else 
    a = 10;
    
assert_equal(a, 34);

if (false)
    a = 1;
else if (false)
    a = 2;
else if (false)
    a = 34;
else if (false)
    a = 240;
else if (false)
    a = 20;
else if (true)
    a = 5;
else if (false)
    a = 50;
else 
    a = 10;

assert_equal(a, 5);

if (false)
    a = 1;
else if (false)
    a = 2;
else if (false)
    a = 34;
else if (false)
    a = 240;
else if (false)
    a = 20;
else if (false)
    a = 5;
else if (false)
    a = 50;
else 
    a = 10;

assert_equal(a, 10);

