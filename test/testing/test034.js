function DaysInYear(y) {
  if (y % 4 != 0) return 365;
  if (y % 4 == 0 && y % 100 != 0) return 366;
  if (y % 100 == 0 && y % 400 != 0) return 365;
  if (y % 400 == 0) return 366;
}
assert_equal(365, DaysInYear(1999));
assert_equal(366, DaysInYear(2000));
