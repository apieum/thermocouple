#ifndef THERMO_H
#define THERMO_H

class Kelvin;
class Celsius
{
    public:
        double temp;
        Celsius(double _temp): temp(_temp) {};
        operator Kelvin();
        operator double() { return temp; };
};

class Kelvin
{
    public:
        double temp;
        Kelvin(double _temp): temp(_temp) {};
        operator Celsius();
        operator double() { return temp; };
};

struct HotCoef
{
    double min_volt;
    double max_volt;
    double min_temp;
    double max_temp;
    double To;
    double Vo;
    double p1;
    double p2;
    double p3;
    double p4;
    double q1;
    double q2;
    double q3;
};
struct ColdCoef
{
    double min_temp;
    double max_temp;
    double To;
    double Vo;
    double p1;
    double p2;
    double p3;
    double p4;
    double q1;
    double q2;
};

struct TempCoefs
{
    ColdCoef cold;
    HotCoef hot[5];
};

extern TempCoefs TypeE;
extern bool operator==(const HotCoef& lhs, const HotCoef& rhs);

template <TempCoefs &ThermoType, typename TempUnit=Celsius>
class Thermocouple
{
    public:
        static bool coef_is_set(HotCoef coef)
        {
            return (coef.min_temp != coef.max_temp);
        }
        static bool lower_or_equal_than_max_temp(double temp, HotCoef coef)
        {
            return (temp <= coef.max_temp);
        }
        static bool lower_or_equal_than_max_volt(double voltage, HotCoef coef)
        {
            return (voltage <= coef.max_volt);
        }
        virtual HotCoef search_coef_where(double value, bool (*contains)(double, HotCoef))
        {
            HotCoef last_valid;
            for (unsigned int i=0; i<5; i++)
            {
                if (contains(value, ThermoType.hot[i]))
                    return ThermoType.hot[i];
                else if(coef_is_set(ThermoType.hot[i]))
                    last_valid = ThermoType.hot[i];
            }
            return last_valid;
        }

        virtual HotCoef coefs_from_temp(double temp)
        {
            Celsius c_temp = TempUnit(temp);
            return search_coef_where(c_temp, (&lower_or_equal_than_max_temp));
        }

        virtual HotCoef coefs_from_volt(double voltage)
        {
            return search_coef_where(voltage, (&lower_or_equal_than_max_volt));
        }

        virtual double cold_temp_to_volt(double temp)
        {
            Celsius Tj = TempUnit(temp);
            ColdCoef c = ThermoType.cold;
            double Dt = Tj - c.To;
            double pj = Dt * (c.p1 + Dt * (c.p2 + Dt * (c.p3 + Dt * c.p4)));
            double qj= 1 + Dt * (c.q1 + Dt * c.q2);
            return c.Vo + pj / qj;
        }
        virtual double hot_volt_to_temp(double voltage)
        {
            HotCoef c = coefs_from_volt(voltage);
            double Dv = voltage - c.Vo;
            double pj = Dv * (c.p1 + Dv * (c.p2 + Dv * (c.p3 + Dv * c.p4)));
            double qj= 1 + Dv * (c.q1 + Dv * (c.q2 + Dv * c.q3));
            TempUnit Tj = Celsius(c.To + pj / qj);
            return Tj;
        }

        virtual double temperature(double voltage, double temp=TempUnit(Celsius(20.0)))
        {
            double Vcj = cold_temp_to_volt(temp);
            return hot_volt_to_temp(voltage + Vcj);
        }

};

#endif

