#include "NavierStokes.h"
#include <cstddef>
#include <stdio.h>
//Public

NavierStokes::NavierStokes(int nx, int ny)
    : Fx(1), Fy(0), rho(1), nu(0.1), 
	  dt(0.001), dx(0.05), dy(0.05), nx(nx), ny(ny),
      err_x(0), err_y(0),
	  b(NULL), p(NULL), u(NULL), v(NULL), cu(NULL), cv(NULL)
{
	b = alloc(0);
	p = alloc(1);
	u = alloc(0);
	v = alloc(0);
	cu = alloc(0);
	cv = alloc(0);
}

NavierStokes::~NavierStokes()
{
	del(b); del(p); del(u); del(v);
}


void 
NavierStokes::next() {
	copy(u, cu);
	copy(v, cv);
    next_b();
    next_p();
    next_u(cu, cv);
    next_v(cu, cv);
}

double NavierStokes::getErrorX() { return err_x; }
double NavierStokes::getErrorY() { return err_y; }

void
NavierStokes::save() 
{
    FILE *fp, *fu, *fv;
    fp = fopen("log/pressure.out", "w");
    fu = fopen("log/velocity_x.out", "w");
    fv = fopen("log/velocity_y.out", "w");
    for(int i = 1; i < nx-1; ++i) {
        for(int j = 0; j < ny; ++j) {
            fprintf(fp, "%f ", p[i][j]);
            fprintf(fu, "%f ", u[i][j]);
            fprintf(fv, "%f ", v[i][j]);
        }
        fprintf(fp, "\n");
        fprintf(fu, "\n");
        fprintf(fv, "\n");
    }
    fclose(fp);
    fclose(fu);
    fclose(fv);
}

//Private
double**
NavierStokes::alloc(double def)
{
	double** matrix = new double *[nx];
	for (int i = 0; i < nx; ++i) {
		matrix[i] = new double[ny];
		for (int j = 0; j < ny; ++j) {
			matrix[i][j] = def;
		}
	}
	return matrix;
}

void
NavierStokes::del(double **matrix)
{
	for (int i = 0; i < nx; ++i) {
		delete[] matrix[i];
	}
	delete[] matrix;
}

void
NavierStokes::copy(double **a, double **ca)
{
	for (int i = 0; i < nx; ++i) {
		for (int j = 0; j < ny; ++j) {
			ca[i][j] = a[i][j];
		}
	}
}

void 
NavierStokes::next_b() 
{
    for(int i = 1; i < nx-1; ++i) {
        for(int j = 1; j < ny-1; ++j) {
            b[i][j] = ((u[i+1][j]-u[i-1][j])/(2*dx) + (v[i][j+1]-v[i][j-1])/(2*dy))/dt
                - (u[i+1][j]-u[i-1][j])/(2*dx)*(u[i+1][j]-u[i-1][j])/(2*dx)
                - 2*(u[i][j+1]-u[i][j-1])/(2*dy)*(v[i+1][j]-v[i-1][j])/(2*dx)
                - (v[i][j+1]-v[i][j-1])/(2*dy)*(v[i][j+1]-v[i][j-1])/(2*dy);
            b[i][j] *= rho;
        }
    }
}

void 
NavierStokes::next_p() 
{
    static double dx2 = dx*dx, dy2 = dy*dy;
    for(int i = 1; i < nx-1; ++i) {
        for(int j = 1; j < ny-1; ++j) {
            b[i][j] = (p[i+1][j]+p[i-1][j])*dy2 
                    + (p[i][j+1]+p[i][j-1])*dx2
                    - b[i][j]*dx2*dy2;
            b[i][j] /= (2*(dx2 + dy2));
        }
    }
	copy(b, p);
    // Wall boundary conditions, pressure
    for(int i = 0; i < nx; ++i) {
        p[i][0] = p[i][1]; // dx/dy=0 at y = 0
        p[i][nx-1] = p[i][nx-2]; // dx/dy=0 at y = max_y
    }
}

void
NavierStokes::next_u(double **cu, double **cv)
{
	for (int i = 1; i < nx - 1; ++i) {
		for (int j = 1; j < ny - 1; ++j) {
			u[i][j] = cu[i][j]
				- cu[i][j] * dt / dx * (cu[i][j] - cu[i - 1][j])
				- cv[i][j] * dt / dy * (cu[i][j] - cu[i][j - 1])
				- dt / (rho*2*dx) * (p[i + 1][j] - p[i - 1][j])
				+ nu * (dt / (dx*dx)*(cu[i + 1][j] - 2 * cu[i][j] + cu[i - 1][j])
					 +  dt / (dy*dy)*(cu[i][j + 1] - 2 * cu[i][j] + cu[i][j - 1]))
				+ dt * Fx;
		}
	}
	for (int i = 0; i < nx-1; ++i) {
		// a = 0 at y = 0, max_y
		u[i][0] = 0;
		u[i][ny - 1] = 0;
	}
	double sum = 0;
	for (int i = 1; i < nx-1; ++i) {
		for (int j = 0; j < ny; ++j) {
			sum += (u[i][j] - cu[i][j])*(u[i][j] - cu[i][j]);
		}
	}
	err_x = sum;
}

void
NavierStokes::next_v(double **cu, double **cv)
{
	for (int i = 1; i < nx - 1; ++i) {
		for (int j = 1; j < ny - 1; ++j) {
			v[i][j] = cv[i][j]
				- cu[i][j] * dt / dx * (cv[i][j] - cv[i - 1][j])
				- cv[i][j] * dt / dy * (cv[i][j] - cv[i][j - 1])
				- dt / (rho * 2 * dy) * (p[i][j+1] - p[i][j-1])
				+ nu * (dt / (dx*dx)*(cv[i + 1][j] - 2 * cv[i][j] + cv[i - 1][j])
					 +  dt / (dy*dy)*(cv[i][j + 1] - 2 * cv[i][j] + cv[i][j - 1]))
				+ dt * Fy;
		}
	}
	for (int i = 0; i < nx; ++i) {
		// v = 0 at y = 0, max_y
		v[i][0] = 0;
		v[i][ny - 1] = 0;
	}
	double sum = 0;
	for (int i = 1; i < nx-1; ++i) {
		for (int j = 0; j < ny; ++j) {
			sum += (v[i][j] - cv[i][j])*(v[i][j] - cv[i][j]);
		}
	}
	err_y = sum;
}
