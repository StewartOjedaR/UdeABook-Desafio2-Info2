#ifndef FECHA_H
#define FECHA_H
typedef unsigned int uint;
typedef unsigned short int ushort;
typedef unsigned long long ulong;
struct Fecha {
    ushort anio;
    ushort mes;
    ushort dia;
    ushort hora;
    ushort minuto;

    bool esAnterior(const Fecha& otra) const {
        if (anio != otra.anio) return anio < otra.anio;
        if (mes != otra.mes) return mes < otra.mes;
        if (dia != otra.dia) return dia < otra.dia;
        if (hora != otra.hora) return hora < otra.hora;
        return minuto < otra.minuto;
    }

    bool esPosterioOIgual(const Fecha& otra) const {
        return !esAnterior(otra);
    }

    bool esAnteriorOIgual(const Fecha& otra) const {return esAnterior(otra) || (anio == otra.anio && mes == otra.mes && dia == otra.dia && hora == otra.hora && minuto == otra.minuto);
    };
};
#endif // FECHA_H
