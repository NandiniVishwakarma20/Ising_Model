#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <cmath>
#include <ctime>

using namespace std;

const int L = 20;            
const double J = 1.0;        
const double h = 0.0;        
const int n_steps = 100000;  
const int equilibration_steps = 5000;  
mt19937 rng(time(0));
uniform_real_distribution<double> uniform(0.0, 1.0);
uniform_int_distribution<int> random_index(0, L-1);

vector<vector<int>> lattice(L, vector<int>(L));

void initialize_lattice() {
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < L; ++j) {
            lattice[i][j] = (uniform(rng) < 0.5) ? 1 : -1;
        }
    }
}

bool is_valid_site(int i, int j) {
    return (i >= 0 && i < L && j >= 0 && j < L);
}

double calculate_local_energy(int i, int j) {
    int spin = lattice[i][j];
    int neighbors = 0;
    
    if (is_valid_site(i+1, j)) neighbors += lattice[i+1][j];
    if (is_valid_site(i-1, j)) neighbors += lattice[i-1][j];
    if (is_valid_site(i, j+1)) neighbors += lattice[i][j+1];
    if (is_valid_site(i, j-1)) neighbors += lattice[i][j-1];
    
    return -J * spin * neighbors - h * spin;
}

double calculate_total_energy() {
    double total_energy = 0.0;
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < L; ++j) {
            total_energy += calculate_local_energy(i, j);
        }
    }
    return total_energy / 2.0;
}

double calculate_magnetization() {
    int total_spin = 0;
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < L; ++j) {
            total_spin += lattice[i][j];
        }
    }
    return static_cast<double>(total_spin) / (L * L);
}

void metropolis(double T) {
    for (int k = 0; k < L * L; ++k) {
        int i = random_index(rng);
        int j = random_index(rng);
        
        double dE = -2 * calculate_local_energy(i, j);
        
        if (dE <= 0 || uniform(rng) < exp(-dE / T)) {
            lattice[i][j] *= -1;
        }
    }
}

int main() {
    ofstream file("ising_free_bc_observables.txt");
    
    double T_start = 1.5;
    double T_end = 3.5;
    double T_step = 0.1;
    
    for (double T = T_start; T <= T_end; T += T_step) {
        initialize_lattice();
        
        for (int step = 0; step < equilibration_steps; ++step) {
            metropolis(T);
        }
        
        double energy_sum = 0.0;
        double energy2_sum = 0.0;
        double magnetization_sum = 0.0;
        double magnetization2_sum = 0.0;
        int measurements = 0;
        
        for (int step = 0; step < n_steps; ++step) {
            metropolis(T);
            
            if (step % 100 == 0) {
                double E = calculate_total_energy();
                double M = calculate_magnetization();
                
                energy_sum += E;
                energy2_sum += E * E;P
                magnetization_sum += abs(M);
                magnetization2_sum += M * M;
                measurements++;
            }
        }
        

        double avg_E = energy_sum / measurements;
        double avg_E2 = energy2_sum / measurements;
        double avg_M = magnetization_sum / measurements;
        double avg_M2 = magnetization2_sum / measurements;
        
        double C = (avg_E2 - avg_E * avg_E) / (T * T);
        double chi = (avg_M2 - avg_M * avg_M) / T;
        
        file << T << " " << avg_E << " " << avg_M << " " << C << " " << chi << endl;
        cout << "T = " << T << " <E> = " << avg_E << " <M> = " << avg_M << " C = " << C << " Chi = " << chi << endl;
    }
    
    file.close();
    return 0;
}