#include <vector>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

using Matrix = vector<vector<double>>;

#define STD_ERR_VAL 5e-4

Matrix readMatrix(string filename, bool augmented = true);
void printInverseMatrix(Matrix mat, string filename);
void printMatrix(Matrix mat);
bool equalMatrices(const Matrix &a, const Matrix &b, double eps);
void last_half(Matrix &mat);
void main_loop(void (*findInvMatGaussJordan)(Matrix&), string file);