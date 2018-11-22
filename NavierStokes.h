#ifndef NAVIERSTOKES_H
#define NAVIERSTOKES_H 1

class NavierStokes {
public:
    NavierStokes(int nx = 41, int ny = 41);
	~NavierStokes();
    void next();
    double getErrorX();
    double getErrorY();
    void save();    

protected:
    int nx, ny;
    double **b, **p, **u, **v, **cu, **cv;
    double err_x, err_y;

private:
    double Fx, Fy, rho, nu, dt, dx, dy;
	
	double** alloc(double def);
	void del(double **matrix);
	void copy(double **a, double **ca);
    void next_b();
    void next_p();
	void next_u(double **cu, double **cv);
	void next_v(double **cu, double **cv);
};

#endif