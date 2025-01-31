#include <stdio.h>
#include <math.h>
#include <omp.h>



const double Vmin = -1.5;
const double Vmax = 1.5;
const double rmax = 5;
const int rsteps = 4096;
const int thetasteps = 4096;
const int bins = 200;

#define PI 3.141592654

#define max(a,b) ((a)>(b)? (a) : (b))

// description profils de densité
const double r0 = 1;
const double r1 = 0.1;

// masse
double density(double r)
{
    return exp(-r/r0);
}

// masse lumineuse HI
double lum_density(double r)
{
    return exp(-r/r1);
}

int findbin(double v)
{
    return (int)(double(bins) * (v-Vmin)/(Vmax-Vmin));
}

int main()
{
    const double dr = rmax / double(rsteps);
    const double dtheta = 2 * PI / double(thetasteps);
    double m = 0;

    // vecteur directeur ligne de visée. A normaliser

    /*const double los_x = 1.0/sqrt(3);
    const double los_y = 1.0/sqrt(3);
    const double los_z = 1.0/sqrt(3);*/

    const double los_x = 1.0;
    const double los_y = 0;
    const double los_z = 0;

    double *I = new double[bins];
    for(int i = 0; i < bins; ++i)
    {
        I[i] = 0;
    }

    // pour chaque r entre 0 et Rmax
    for(int i = 0; i < rsteps; ++i)
    {
        double r = double(i) * dr;
        m += density(r) * 2 * PI * r * dr; 

        // pour chaque theta entre 0 et 2 pi
        for(int j = 0; j < thetasteps; ++j) if(r > 0.0001)
        {
            // calcul de la vitesse vectorielle du flot de matière en ce point (r, theta)
            double theta = double(j) * dtheta;
            double vr = sqrt(m/r);
            double vx = -vr * sin(theta), vy = vr * cos(theta);
            double v = vx * los_x + vy * los_y;
            int bin = findbin(v);
            if(bin < 0 || bin >= bins)
            {
                printf("value out of range %f %f (%d)\n", v, m, bin);
                continue;
            }
            // ajoute du signal proportionnelle à la quantité de matière lumineuse dans l'élément de surface [r, r+dr[ x [theta, theta + dtheta[, 
            // dans l'intervalle [v, v+dv[
            I[bin] += lum_density(r) * r * dr * dtheta;
        }
    }


    FILE *fp = fopen("curve.res", "w+");
    for(int i = 0; i < bins; ++i)
    {
        fprintf(fp, "%f %f\n", Vmin + (Vmax - Vmin) * double(i) / double(bins), I[i]);
    }
    fclose(fp);
}
