/* Mandelbrot! */

X1 = -2.0;
Y1 = -2.0;
X2 = 2.0;
Y2 = 2.0;
PX = 32;
PY = 32;


lines = new Array(33);
for (y = 0; y < PY; y++) {
	line = [];
	for (x = 0; x < PX; x++) {
		Xr = 0;
		Xi = 0;
		Cr = X1 + ((X2 - X1) * x / PX);
		Ci = Y1 + ((Y2 - Y1) * y / PY);
		iterations = 0;
		while ((iterations < 32) && ((Xr * Xr + Xi * Xi) < 4)) {
			t = Xr * Xr - Xi * Xi + Cr;
			Xi = 2 * Xr * Xi + Ci;
			Xr = t;
			iterations++;
		}
		if (iterations & 1)
			line.push("*");
		else
			line.push(" ");
	}
	lines[y] = line.join("");
}

assert_equal(lines[0], "********************************");
assert_equal(lines[1], "***********           **********");
assert_equal(lines[2], "*********               ********");
assert_equal(lines[3], "*******                   ******");
assert_equal(lines[4], "******                     *****");
assert_equal(lines[5], "*****                       ****");
assert_equal(lines[6], "****     *******             ***");
assert_equal(lines[7], "***   ******* ** **           **");
assert_equal(lines[8], "***  ******  * *   *          **");
assert_equal(lines[9], "** *******   ** **  **         *");
assert_equal(lines[10], "** ******  * *   ** **         *");
assert_equal(lines[11], "* *****  ***      ** **         ");
assert_equal(lines[12], "****** ***         *****        ");
assert_equal(lines[13], "***  * * *         ** **        ");
assert_equal(lines[14], "*   * *   *         * **        ");
assert_equal(lines[15], "*   ***            ** **        ");
assert_equal(lines[16], "*                  ** **        ");
assert_equal(lines[17], "*   ***            ** **        ");
assert_equal(lines[18], "*   * *   *         * **        ");
assert_equal(lines[19], "***  * * *         ** **        ");
assert_equal(lines[20], "****** ***         *****        ");
assert_equal(lines[21], "* *****  ***      ** **         ");
assert_equal(lines[22], "** ******  * *   ** **         *");
assert_equal(lines[23], "** *******   ** **  **         *");
assert_equal(lines[24], "***  ******  * *   *          **");
assert_equal(lines[25], "***   ******* ** **           **");
assert_equal(lines[26], "****     *******             ***");
assert_equal(lines[27], "*****                       ****");
assert_equal(lines[28], "******                     *****");
assert_equal(lines[29], "*******                   ******");
assert_equal(lines[30], "*********               ********");
assert_equal(lines[31], "***********           **********");
