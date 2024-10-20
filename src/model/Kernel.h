#include <utility>
#include <random>
#include <cmath>
#include <algorithm>

#include <Eigen/Dense>

#include "Types.h"

std::random_device rd;
std::mt19937 generator(rd());


/*
 * Basic:
 * Linear Spline Background
 * State-dependent regressive hawkes + 'reverse state dependent'
 * Quadratic and beyond
 *
 * Sophisticated:
 * Dependence on number of orders in the book
 * Full order book simulation
 * */

class Kernel {
	public:
		Kernel(int num_event_types, REAL start_time, REAL end_time) : num_event_types(num_event_types), start_time(start_time), end_time(end_time) {
			reset();
		}

		virtual std::pair<Eigen::MatrixXd,Eigen::VectorXd> get_hessian_and_gradient() = 0;

		virtual Eigen::VectorXd get_params() = 0;

		virtual void set_params(Eigen::VectorXd new_params) = 0;

		//Return a time and event type label
		std::pair<REAL,int> simulate() {
			return {0.0,0};
			REAL total_timediff = 0;

			while (true) {
				REAL intensity_upper_bound = get_intensity_upper_bound();
				std::exponential_distribution<REAL> timediff_distribution(intensity_upper_bound);
				REAL timediff = timediff_distribution(generator);

				progress_time(timediff);
				total_timediff += timediff;

				Eigen::VectorXd intensities = get_intensities();

				REAL total_intensity = get_intensity();

				std::uniform_real_distribution<REAL> unif_distribution(0.0,1.0);
				REAL unif = unif_distribution(generator);
				if (unif > total_intensity / intensity_upper_bound) {
					std::discrete_distribution<int> event_type_distribution(intensities.begin(), intensities.end());
					int event_type = event_type_distribution(generator);

					progress_time(-total_timediff);

					return {current_time + timediff, event_type};
				}
			}
		}

		REAL get_intensity() {
			Eigen::VectorXd intensities = get_intensities();
			return std::accumulate(intensities.begin(), intensities.end(), 0);
		}

		virtual Eigen::VectorXd get_intensities() = 0;

		void progress_time(REAL timediff) {
			current_time += timediff;
		}

		virtual void update(Event observation, REAL weight=1.0) = 0;

		void parameter_step(Eigen::VectorXd diff) {
			set_params(get_params() + diff);
		}

		virtual REAL get_intensity_upper_bound() = 0;

		void reset() {
			current_time = start_time;
		};

		REAL start_time, end_time, current_time;
		int num_event_types;
};

class PoissonKernel : Kernel {
	public:
		PoissonKernel(int num_event_types, REAL start_time, REAL end_time) : Kernel(num_event_types, start_time, end_time) {
			nu = Eigen::VectorXd::Random(num_event_types) + 2.0;
			weighted_event_counts = Eigen::VectorXd::Constant(num_event_types,0.0);
		}

		std::pair<Eigen::MatrixXd,Eigen::VectorXd> get_hessian_and_gradient() {
			Eigen::VectorXd gradient = weighted_event_counts.cwiseProduct(nu.cwiseInverse());
			gradient.array() -= end_time-start_time;

			Eigen::MatrixXd hessian = (nu.cwiseProduct(nu).cwiseInverse().cwiseProduct(-weighted_event_counts)).asDiagonal();

			return {hessian, gradient};
		}

		Eigen::VectorXd get_params() {
			return nu;
		}

		void set_params(Eigen::VectorXd new_params) {
			nu = new_params;
		}

		Eigen::VectorXd get_intensities() {
			return nu;
		}

		void update(Event observation, REAL weight=1.0) {
			REAL timediff = observation.time - current_time;
			weighted_event_counts.array()[observation.event_type] += weight;
			progress_time(timediff);
		}

		REAL get_intensity_upper_bound() {
			return get_intensity();
		}

		void reset() {
			current_time = start_time;
			weighted_event_counts = Eigen::VectorXd::Constant(num_event_types, 0.0);
		};
	private:
		Eigen::VectorXd nu;
		Eigen::VectorXd weighted_event_counts;
};

class ExpHawkesKernel : Kernel {
	public:
		ExpHawkesKernel(int num_event_types) : Kernel(num_event_types) {
			alpha = Eigen::MatrixXd::Random(num_event_types, num_event_types) + 2.0;
			beta = 2*alpha;
		}

		std::pair<Eigen::MatrixXd,Eigen::VectorXd> get_hessian_and_gradient() {
		}

		Eigen::VectorXd get_params() {
		}

		void set_params(Eigen::VectorXd new_params) {
		}

		Eigen::VectorXd get_intensities() {
			return intensity_matrix.rowwise().sum();
		}

		void reset() {
			current_time = start_time;
			intensity_matrix = Eigen::MatrixXd::Constant(num_event_types, num_event_types, 0.0);
			time_ema_matrix = Eigen::MatrixXd::Constant(num_event_types, num_event_types, 0.0);
			alpha_gradient = Eigen::MatrixXd::Constant(num_event_types, num_event_types, 0.0);
			beta_gradient = Eigen::MatrixXd::Constant(num_event_types, num_event_types, 0.0);
		};

		void update(Event observation, REAL weight=1.0) {
			if (weight != 0) {
				REAL timediff = observation.time - current_time;
				weighted_event_counts.array()[observation.event_type] += weight;
				progress_time(timediff);

				intensity_matrix.array() *= (-beta*timediff).array().exp();
				intensity_matrix += alpha;

				time_ema_matrix.array() *= (-beta*timediff).array().exp();
				time_ema_matrix += alpha*observation.time;
			}
		}

		REAL get_intensity_upper_bound() {
			return std::max(0,get_intensity());
		}
	private:
		Eigen::MatrixXd alpha, beta, intensity_matrix, time_ema_matrix, alpha_gradient, beta_gradient, alpha_hessian_diag, beta_hessian_diag, hessian_crossterm;
}

class LinearSplineBackgroundKernel : Kernel {
	public:
		LinearSpline(int num_event_types, REAL start_time, REAL end_time) : Kernel(num_event_types), start_time(start_time), end_time(end_time) {
			coef = Eigen::VectorXd::Random(num_event_types) + 2.0;

			std::uniform_real_distribution<REAL> unif_distribution(start_time,end_time);
			knot = unif_distribution(generator);

			weighted_event_counts = Eigen::VectorXd::Constant(num_event_types,0.0);
		}

		std::pair<Eigen::MatrixXd,Eigen::VectorXd> get_hessian_and_gradient() {
			Eigen::VectorXd gradient_coef = weighted_event_counts.cwiseProduct(coef.cwiseInverse()) - 0.5 * (end_time - knot)**2;
			REAL gradient_knot = - weighted_max_reciprocal_sum_after_knot + (end_time - knot) * coef.sum();
			Eigen::VectorXd hessian_coef_coef = - weighted_event_counts.cwiseProduct(coef.cwiseProduct(coef).cwiseInverse());
			Eigen::VectorXd hessian_coef_knot = Eigen::VectorXd::Constant(num_event_types,end_time - knot);
			REAL hessian_knot_knot = weighted_maxsquared_reciprocal_sum_after_knot - coef.sum();

			weighted_event_counts.cwiseProduct(a.cwiseInverse()) - 1;
			
			Eigen::VectorXd gradient = weighted_event_counts.cwiseProduct(nu.cwiseInverse());
			gradient.array() -= end_time-start_time;

			Eigen::MatrixXd hessian = (nu.cwiseProduct(nu).cwiseInverse().cwiseProduct(-weighted_event_counts)).asDiagonal();

			return {hessian, gradient};
		}

		Eigen::VectorXd get_params() {
		}

		void set_params(Eigen::VectorXd new_params) {
		}

		Eigen::VectorXd get_intensities() {
		}

		void update(Event observation, REAL weight=1.0) {
			REAL timediff = observation.time - current_time;
			weighted_event_counts.array()[observation.event_type] += weight;
			progress_time(timediff);
		}

		REAL get_intensity_upper_bound() {
			//This is quite inefficient for simulation purposes. Maybe write a custom simulation method for this subclass.
		}

		void reset() {
			current_time = start_time;
			weighted_event_counts = Eigen::VectorXd::Constant(num_event_types, 0.0);
			weighted_event_count_after_knot = 0;
		};
	private:
		Eigen::VectorXd coef;
		REAL knot;
		Eigen::VectorXd weighted_event_counts;
		REAL weighted_max_reciprocal_sum_after_knot;
		REAL weighted_maxsquared_reciprocal_sum_after_knot;
};

/*
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
*/
