#include "lib/liback/util.h"

// since decimal point numbers are not supported in kernel mode, these functions
// provide solutions to common problems without requiring such arithmetic

intmax_t fraction_to_percentage(intmax_t num, intmax_t dem);
