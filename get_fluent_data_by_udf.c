// Coding: GBK

#include "udf.h"
#include "cxndsearch.h"

static ND_Search *domain_table = NULL;

void get_parallel_data(double *Temperature, double *Pressure, double Velocity[], double *Density)
{
    double Temperature_parallel = 0.0;
    double Pressure_paralle = 0.0;
    double Velocity_u_paralle = 0.0;
    double Velocity_v_paralle = 0.0;
    double Velocity_w_paralle = 0.0;
    double Density_paralle = 0.0;
    //double Temperature = 0.0;

#if !RP_HOST
    cell_t c;
    Thread *t;
    CX_Cell_Id *cx_cell;
    double velocity;

    double P[3] = {60.0, 0.0, 0.0};
    double P_Cell[3];

    domain_table = CX_Start_ND_Point_Search(domain_table, TRUE, -1);

    cx_cell = CX_Find_Cell_With_Point(domain_table, P, 0.0);

    if (cx_cell)
    {
        c = RP_CELL(cx_cell);
        t = RP_THREAD(cx_cell);
        Temperature_parallel = C_T(c, t);
        Pressure_paralle = C_P(c, t);
        Velocity_u_paralle = C_U(c, t);
        Velocity_v_paralle = C_V(c, t);
        Velocity_w_paralle = C_W(c, t);
        Density_paralle = C_R(c, t);
    }

    domain_table = CX_End_ND_Point_Search(domain_table);
#if RP_NODE
    Temperature_parallel = PRF_GRSUM1(Temperature_parallel);
    Pressure_paralle = PRF_GRSUM1(Pressure_paralle);
    Velocity_u_paralle = PRF_GRSUM1(Velocity_u_paralle);
    Velocity_v_paralle = PRF_GRSUM1(Velocity_v_paralle);
    Velocity_w_paralle = PRF_GRSUM1(Velocity_w_paralle);
    Density_paralle = PRF_GRSUM1(Density_paralle);
#endif

#endif

    node_to_host_double_6(Temperature_parallel, Pressure_paralle, Velocity_u_paralle, Velocity_v_paralle, Velocity_w_paralle, Density_paralle);
    *Temperature = Temperature_parallel;
    *Pressure = Pressure_paralle;
    Velocity[0] = Velocity_u_paralle;
    Velocity[1] = Velocity_v_paralle;
    Velocity[2] = Velocity_w_paralle;
    *Density = Density_paralle;
    
}

DEFINE_ON_DEMAND(get_fluent_data)
{
    double Temperature = 10.0;
    double Pressure = 10.0;
    double Velocity[3] = {0.0, 0.0, 0.0};
    double Density = 10.0;
    get_parallel_data(&Temperature, &Pressure, Velocity, &Density);
#if RP_HOST
    Message("Temperature = %f\n", Temperature);
    Message("Pressure = %f\n", Pressure);
    Message("Velocity_x = %f\n", Velocity[0]);
    Message("Velocity_y = %f\n", Velocity[1]);
    Message("Velocity_z = %f\n", Velocity[2]);
    Message("Density = %f\n", Density);
#endif
}