#ifndef SISTEMA_H
#define SISTEMA_H
//#include <iostream>
#include <fstream>
//#include "album.h"
//#include "mensaje.h"
#include "usuario.h"
using namespace std;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long long ulong;
int longitudCadena(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}
class Sistema {
private:
    Usuario* usuarioActual;
    fstream* baseDatosUtilizada;
    uint cantidadDeUsuarios;
    bool sesionIniciada;
    size_t calcularBytesMatriz(unsigned int n) const;  // Calcula el número total de bytes necesarios para la matriz de bits (N * N bits / 8)
    char* nombreArchivoMatriz;

public:
    Sistema(const char* archivoMatriz);
    ~Sistema();    // Destructor para liberar la memoria dinámica del nombre y de los flujos
    bool abrirBaseDatos();
    void cerrarBaseDatos();
    void registrarNuevoUsuario();  // Registra un nuevo usuario, expandiendo dinámicamente la matriz de adyacencia en disco
    void agregarAmistad(uint id1, uint id2);// Establece amistad entre dos usuarios usando operaciones de bits
    void sugerirAmigos(uint idUsuario); // Sugiere amigos encontrando amigos en común mediante operaciones de bits
    bool estaSesionIniciada() const { return sesionIniciada; }
    uint getCantidadDeUsuarios() const { return cantidadDeUsuarios; }
};

Sistema::Sistema(const char* archivoMatriz) {
    usuarioActual = nullptr;
    sesionIniciada = false;
    baseDatosUtilizada = nullptr;
    cantidadDeUsuarios = 0;

    // Reserva dinámica de memoria para el nombre del archivo usando la longitud exacta
    int tam = longitudCadena(archivoMatriz);
    nombreArchivoMatriz = new char[tam + 1];

    // Copia manual carácter por carácter
    for (int i = 0; i <= tam; ++i) {
        nombreArchivoMatriz[i] = archivoMatriz[i];
    }

    // Asegurar que el archivo matriz exista al iniciar
    std::ifstream test(nombreArchivoMatriz, std::ios::binary);
    if (!test.is_open()) {
        std::ofstream creador(nombreArchivoMatriz, std::ios::binary);
        creador.close();
    } else {
        test.close();
    }
}

Sistema::~Sistema() {
    cerrarBaseDatos();
    if (nombreArchivoMatriz != nullptr) {
        delete[] nombreArchivoMatriz;
        nombreArchivoMatriz = nullptr;
    }
}
bool Sistema::abrirBaseDatos() {
    cerrarBaseDatos();
    baseDatosUtilizada = new fstream(nombreArchivoMatriz, std::ios::in | std::ios::out | std::ios::binary);
    return baseDatosUtilizada->is_open();
}
void Sistema::cerrarBaseDatos() {
    if (baseDatosUtilizada != nullptr) {
        if (baseDatosUtilizada->is_open()) {
            baseDatosUtilizada->close();
        }
        delete baseDatosUtilizada;
        baseDatosUtilizada = nullptr;
    }
}
void Sistema::registrarNuevoUsuario() {
    cantidadDeUsuarios++;

    ofstream archivo(nombreArchivoMatriz, std::ios::in | std::ios::out | std::ios::binary);
    if (!archivo.is_open()) return;

    size_t nuevosBytes = calcularBytesMatriz(cantidadDeUsuarios);

    archivo.seekp(nuevosBytes - 1, std::ios::beg);
    char cero = 0;
    archivo.write(&cero, 1);
    archivo.close();

    cout << "Nuevo usuario registrado. Total usuarios: " << cantidadDeUsuarios << "\n";
}
void Sistema::agregarAmistad(uint id1, uint id2) {
    if (id1 >= cantidadDeUsuarios || id2 >= cantidadDeUsuarios) return;

    ulong bitGlobal = static_cast<ulong>(id1) * cantidadDeUsuarios + id2;
    ulong bitInverso = static_cast<ulong>(id2) * cantidadDeUsuarios + id1;

    fstream archivo(nombreArchivoMatriz, std::ios::in | std::ios::out | std::ios::binary);
    if (!archivo.is_open()) return;

    ulong indices[2] = {bitGlobal, bitInverso};
    for (int k = 0; k < 2; ++k) {
        ulong indiceByte = indices[k] / 8;
        uint desplazamientoEnByte = indices[k] % 8;

        archivo.seekg(indiceByte, std::ios::beg);
        uchar byteActual = 0;
        archivo.read(reinterpret_cast<char*>(&byteActual), 1);

        byteActual |= (1 << desplazamientoEnByte);

        archivo.seekp(indiceByte, std::ios::beg);
        archivo.write(reinterpret_cast<const char*>(&byteActual), 1);
    }
    archivo.close();
}
void Sistema::sugerirAmigos(uint idUsuario) {
    if (idUsuario >= cantidadDeUsuarios) return;

    cout << "Sugerencias de amigos para el usuario " << idUsuario << ": \n";

    ifstream archivo(nombreArchivoMatriz, std::ios::binary);
    if (!archivo.is_open()) return;

    for (uint candidato = 0; candidato < cantidadDeUsuarios; ++candidato) {
        if (candidato == idUsuario) continue;

        ulong bitDirecto = static_cast<ulong>(idUsuario) * cantidadDeUsuarios + candidato;
        archivo.seekg(bitDirecto / 8, std::ios::beg);
        uchar bDirecto;
        archivo.read(reinterpret_cast<char*>(&bDirecto), 1);
        bool yaSonAmigos = (bDirecto & (1 << (bitDirecto % 8))) != 0;

        if (yaSonAmigos) continue;

        uint amigosEnComun = 0;
        for (uint j = 0; j < cantidadDeUsuarios; ++j) {
            ulong bit1 = static_cast<ulong>(idUsuario) * cantidadDeUsuarios + j;
            archivo.seekg(bit1 / 8, std::ios::beg);
            uchar byte1;
            archivo.read(reinterpret_cast<char*>(&byte1), 1);
            bool amigo1 = (byte1 & (1 << (bit1 % 8))) != 0;

            ulong bit2 = static_cast<ulong>(candidato) * cantidadDeUsuarios + j;
            archivo.seekg(bit2 / 8, std::ios::beg);
            uchar byte2;
            archivo.read(reinterpret_cast<char*>(&byte2), 1);
            bool amigo2 = (byte2 & (1 << (bit2 % 8))) != 0;

            if (amigo1 && amigo2) {
                amigosEnComun++;
            }
        }

        if (amigosEnComun > 0) {
            cout << " - Usuario " << candidato << " (" << amigosEnComun << " amigos en común)\n";
        }
    }
    archivo.close();
}
#endif // SISTEMA_H
