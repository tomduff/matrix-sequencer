
class Utilities {
public:
  static void bound(int &value, int min, int max) {
    if (value < min ) value = min;
    else if (value > max) value = max;
  }
  static void cycle(int &value, int min, int max) {
    if (value < min ) value = max;
    else if (value > max) value = min;
  }
};
