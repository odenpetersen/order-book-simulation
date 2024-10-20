#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

#include <Eigen/Dense>

// Example usage
#include "Parse.h"
#include "Kernel.h"
int main() {
	std::cout << std::setprecision(20);

	PoissonKernel kernel(Eigen::VectorXd::Constant(11, 1.0));
	while (true) {
		Realisation session("../../output/databento/glbx-mdp3-20240913.csv"); // Replace with your CSV file path

		int bucket_size = 1000;
		int bucket_counter = 0;
		for (const auto event : session) {
			kernel.set_start_time((int)((event->time/60/60)+0.5)*60*60);
			kernel.set_current_time(event->time);
			kernel.update(*event);
			bucket_counter++;
			if (bucket_counter >= bucket_size) {
				std::cout << (event->time / seconds_in_day)*100 << "\% done" << std::endl;
				std::cout << *event << std::endl;
				bucket_counter = 0;
			}
		}
		kernel.set_end_time((int)((kernel.current_time/60/60)+0.5)*60*60);

		auto [hess,grad] = kernel.get_hessian_and_gradient();
		std::cout << kernel.start_time << ", " << kernel.end_time << std::endl;
		std::cout << hess << std::endl;
		std::cout << grad << std::endl;
		std::cout << kernel.get_params() << std::endl;
		auto step = -hess.colPivHouseholderQr().solve(grad);
		std::cout << step << std::endl;
		kernel.set_params(kernel.get_params() + step);
		std::cout << kernel.get_params() << std::endl;
	}

	return 0;
}
