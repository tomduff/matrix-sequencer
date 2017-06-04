
class Utilities {
public:
  static void bound(int &value, int min, int max) {
    if (value < min ) value = min;
    else if (value > max) value = max;
  }
  static void bound(long &value, long min, long max) {
    if (value < min ) value = min;
    else if (value > max) value = max;
  }
  static void cycle(int &value, int min, int max) {
    if (value < min ) value = max;
    else if (value > max) value = min;
  }
  static bool reverse(int &value, int min, int max) {
    bool reversed = false;
    if (value > max) {
      value = max;
      reversed = true;
    } else if (value < min) {
      value = min;
      reversed = true;
    }
    return reversed;
  }
};
