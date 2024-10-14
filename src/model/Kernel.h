#include <containers>
#include <random>
#include <cmath>

#define REAL long double
#define VECTOR
#define MATRIX

std::random_device rd;
std::mt19937 generator(rd());

struct Event {
	REAL time;
	int event_type;
	VECTOR<REAL> marks;
	REAL weight;
}

class Kernel {
	public:
		Kernel(REAL start_time, REAL end_time, int num_event_types) : start_time(start_time), end_time(end_time), num_event_types(num_event_types) {
			reset();
		}

		//Returns the information matrix and score for the point process
		virtual std::pair<MATRIX<REAL>,VECTOR<REAL>> get_hessian_and_gradient(GENERATOR<Event> events) = 0;

		virtual VECTOR<REAL> get_params() = 0;

		virtual void set_params(VECTOR<REAL> new_params) = 0;

		//Return a time and event type label
		std::pair<REAL,int> simulate() {
			REAL total_timediff = 0;

			while (true) {
				REAL intensity_upper_bound = get_intensity_upper_bound();
				std::exponential_distribution<REAL> timediff_distribution(intensity_upper_bound);
				REAL timediff = timediff_distribution(generator);

				progress_time(timediff);
				total_timediff += timediff;

				VECTOR<REAL> intensities = get_intensities();

				REAL total_intensity = get_intensity();

				std::uniform_real_distribution<REAL> unif_distribution(0.0,1.0);
				REAL unif = unif_distribution(generator);
				if (unif > total_intensity / intensity_upper_bound) {
					std::discrete_distribution<int> event_type_distribution(intensities);
					int event_type = event_type_distribution(generator);

					progress_time(-total_timediff);

					return std::pair<current_time + timediff, event_type>;
				}
			}
		}

		REAL get_intensity() {
			VECTOR<REAL> intensities = get_intensities();
			return std::accumulate(intensities.begin(), intensities.end(), 0);
		}

		virtual VECTOR<REAL> get_intensities() = 0;

		void reset() {};

		void progress_time(REAL timediff) {
			current_time += timediff;
		}

		void update(Event observation) {
			REAL timediff = observation->time - current_time
			progress_time(timediff);
		}

		void parameter_step(VECTOR<REAL> diff) {
			set_params(get_params() + diff);
		}

	private:
		REAL start_time;
		REAL end_time;
		REAL current_time;
		int num_event_types;
}

class PoissonKernel : Kernel {
	public:
		PoissonKernel(VECTOR<REAL> nu) : nu(nu) {}

		std::pair<MATRIX<REAL>,VECTOR<REAL>> get_hessian_and_gradient(GENERATOR<Event> events) {
			VECTOR<REAL> weighted_num_events(nu.size(), 0);
			for (Event e : events) {
				weighted_num_events[e.event_type] += e.weight;
			}

			// log(nu) * count - T nu
			// count/nu - T
			// -count/nu^2 
			VECTOR<REAL> gradient = count/nu - (end_time-start_time);

			MATRIX<REAL> hessian = (nu.cwiseProduct(nu).cwiseInverse() * (-count)).asDiagonal();

			return std::pair(hessian, gradient);
		}

		VECTOR<REAL> get_params() {
			return nu;
		}

		void set_params(VECTOR<REAL> new_params) {
			nu = new_params;
		}

		virtual VECTOR<REAL> get_intensities() {
			return nu;
		}

	private:
		VECTOR<REAL> nu;
}

class PolynomialBackgroundKernel {
	// a + sum b * (x-c)^k
}

class CompositeKernel {
}

class ExpHawkesKernel {
	public:
		long double nu;
		long double alpha;
		long double beta;

		ExpHawkesKernel(float nu, float alpha, float beta) : nu(nu), alpha(alpha), beta(beta) {
			reset();
		};

		bool first;
		long double start_time;
		long double end_time;

		long double prev_time;
		long double endo_intensity = 0;

		long double total_intensity = 0;

		long double estimated_num_endo = 0;
		long double estimated_num_exo = 0;
		int num_events = 0;
		long double decaying_sum_ti = 0;
		long double beta_denominator = 0;

		void reset() {
			first = true;
			endo_intensity = 0;
			total_intensity = 0;

			estimated_num_endo = 0;
			estimated_num_exo = 0;
			num_events = 0;
			decaying_sum_ti = 0;
			beta_denominator = 0;
		}

		void update(long double time) {
			if (first) {
				start_time = time;
				prev_time = time;
				first = false;
			} else {
				long double decay = beta < 0 ? 1 : std::exp(-beta * (time - prev_time));
				endo_intensity *= decay;
				decaying_sum_ti *= decay;
			}
			endo_intensity += alpha;

			total_intensity = nu + endo_intensity;

			estimated_num_exo += nu / total_intensity;

			estimated_num_endo += endo_intensity / total_intensity;

			num_events++;

			decaying_sum_ti += alpha * time;

			beta_denominator += (time * endo_intensity - decaying_sum_ti) / (nu + endo_intensity);
		}

		bool em_step() {
			long double nu_old = nu;
			long double alpha_old = alpha;
			long double beta_old = beta;

			nu = estimated_num_exo / (end_time - start_time);
			beta = std::abs(estimated_num_endo / beta_denominator);
			alpha = std::abs(beta * estimated_num_endo / num_events);

			std::cerr << nu << ", " << alpha << ", " << beta << std::endl;

			reset();

			return !(std::abs(nu-nu_old) < 1e-4 & std::abs(alpha-alpha_old) < 1e-4 & std::abs(beta-beta_old) < 1e-4);
		}
};
