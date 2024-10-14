#include <iostream>
#include <iomanip>
#include <cmath>

#include "Parse.h"
#include "Kernel.h"

// Example usage
int main() {
	std::cout << std::setprecision(20);
	long double seconds_since_midnight;

	Kernel* kernel = new Kernel(1.0, 1.0, 2.0);
	do {

		Realisation session("../../output/databento/glbx-mdp3-20240806.csv"); // Replace with your CSV file path

		int bucket_size = 100000;
		int curr_bucket = 0;
		for (const auto& event : session) {
			if (event->action[0] == 'T') {
				seconds_since_midnight = event->time - (((int) event->time)/(60*60*24))*60*60*24;
				kernel->update(seconds_since_midnight);
				//std::cout << kernel->num_events << std::endl;
				if (kernel->num_events >= curr_bucket + bucket_size) {
					curr_bucket = kernel->num_events;
					std::cout << kernel->num_events << ", " << kernel->estimated_num_endo + kernel->estimated_num_exo << ", " << kernel->estimated_num_endo << ", " << kernel->estimated_num_exo << ", " << std::endl;
					std::cout << kernel->endo_intensity << ", " << kernel->decaying_sum_ti << ", " << kernel->beta_denominator << std::endl;
				}
			}
		}
		kernel->end_time = seconds_since_midnight;

		std::cerr << kernel->nu << ", " << kernel->alpha << ", " << kernel->beta << std::endl;
		
	} while (kernel->em_step());

	return 0;
}

