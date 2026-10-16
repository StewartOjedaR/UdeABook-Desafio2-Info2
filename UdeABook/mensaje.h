#ifndef MENSAJE_H
#define MENSAJE_H
#include <iostream>
#include <fstream>
using namespace std;
typedef unsigned int uint;
typedef unsigned short int ushort;
typedef unsigned long long ulong;
void copiarCadena(char* destino, const char* origen, int maxLen) {
    int i = 0;
    while (origen[i] != '\0' && i < maxLen - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}
struct Fecha {
    ushort anio;
    ushort mes;
    ushort dia;
    ushort hora;
    ushort minuto;

    bool esAnteriorOIgual(const Fecha& otra) const {
        if (anio != otra.anio) return anio < otra.anio;
        if (mes != otra.mes) return mes < otra.mes;
        if (dia != otra.dia) return dia < otra.dia;
        if (hora != otra.hora) return hora < otra.hora;
        return minuto <= otra.minuto;
    }
};

class Mensaje {
private:
    ulong idMensaje;                 // Identificador unico global del mensaje
    uint idRemitente;                     // ID del usuario que envía
    uint idDestinatario;                  // ID del usuario que recibe (el dueño de la bandeja o el contacto)
    Fecha fecha;                                  // Fecha y hora del mensaje
    char contenido[512];                          // Texto del mensaje (ampliado a 512 bytes)
    bool leido;                                   // Estado (true si ya fue leido)
    long long desplazamientoSiguienteMensaje;     // Desplazamiento en bytes al siguiente mensaje en el archivo (-1 si es el último)



public:
    Mensaje() {
        idMensaje = 0;
        idRemitente = 0;
        idDestinatario = 0;
        fecha = {0, 0, 0, 0, 0};
        for (int i = 0; i < 512; ++i) contenido[i] = '\0';
        leido = false;
        desplazamientoSiguienteMensaje = -1;
    }

    Mensaje(unsigned long long _idMsg, unsigned int _remitente, unsigned int _destinatario,
            const char* _contenido, Fecha _fecha) {
        idMensaje = _idMsg;
        idRemitente = _remitente;
        idDestinatario = _destinatario;
        fecha = _fecha;
        copiarCadena(contenido, _contenido, 512);
        leido = false;
        desplazamientoSiguienteMensaje = -1;
    }

    // Persistencia binaria directa del registro
    void escribirEnArchivo(std::ofstream& archivo) const {
        archivo.write(reinterpret_cast<const char*>(this), sizeof(Mensaje));
    }

    void escribirEnArchivo(std::fstream& archivo) const {
        archivo.write(reinterpret_cast<const char*>(this), sizeof(Mensaje));
    }

    bool leerDeArchivo(std::ifstream& archivo) {
        return archivo.read(reinterpret_cast<char*>(this), sizeof(Mensaje)).good();
    }

    bool leerDeArchivo(std::fstream& archivo) {
        return archivo.read(reinterpret_cast<char*>(this), sizeof(Mensaje)).good();
    }

    // Getters
    unsigned long long getIdMensaje() const { return idMensaje; }
    unsigned int getIdRemitente() const { return idRemitente; }
    unsigned int getIdDestinatario() const { return idDestinatario; }
    Fecha getFecha() const { return fecha; }
    const char* getContenido() const { return contenido; }
    bool estaLeido() const { return leido; }
    long long getDesplazamientoSiguienteMensaje() const { return desplazamientoSiguienteMensaje; }

    // Setters
    void setLeido(bool estado) { leido = estado; }
    void setDesplazamientoSiguienteMensaje(long long desplazamiento) { desplazamientoSiguienteMensaje = desplazamiento; }
};

#endif // MENSAJE_H
