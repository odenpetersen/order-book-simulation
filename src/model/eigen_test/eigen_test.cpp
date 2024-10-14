#include <iostream>
#include <Eigen/Dense>

int main() {
    // Define the coefficient matrix A
    Eigen::Matrix2d A;
    A << 2, 3,
         4, 9;

    // Define the constant vector b
    Eigen::Vector2d b;
    b << 5,
         14;

    // Solve for x
    Eigen::Vector2d x = A.colPivHouseholderQr().solve(b);

    // Output the solution
    std::cout << "Solution:\n" << x << std::endl;

    return 0;
}

