#include "library.h"

#include <iostream>
#include "DatabaseHelper.h"
#include <string>

using namespace std;
const string kDbName = "gis";
const string kUser = "gisuser";
const string kPassword = "wtz2trln";
const string kHostAddress = "localhost";
const string kPort = "5432";

void hello() {
    std::cout << "Hello, World!" << std::endl;
}
