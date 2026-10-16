#ifndef USUARIO_H
#define USUARIO_H
#include <iostream>
using namespace std;
typedef unsigned int uint;
typedef unsigned short int ushort ;
void copiarCadena(char* destino, const char* origen, int maxLen) {
    int i = 0;
    while (origen[i] != '\0' && i < maxLen - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

int longitudCadena(const char* str){
    if (str == nullptr) return 0;
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

char* duplicarCadena(const char* origen) {
    if (origen == nullptr) return nullptr;
    int len = longitudCadena(origen);
    char* destino = new char[len + 1];
    for (int i = 0; i <= len; ++i) destino[i] = origen[i];
    return destino;
}

class Usuario {
private:
    uint ID;
    char nombreUsuario[50];
    char contrasema[20];
    long long desplazamientoPrimerAlbum;
    long long desplazamientoPrimerMensaje;
 //   bool activo;

public:
    Usuario();

    Usuario(uint _id, const char* _nombreUsuario, const char* _contrasema) {
        ID = _id;
        copiarCadena(nombreUsuario,_nombreUsuario,50);
        copiarCadena(contrasema,_contrasema,20);
        desplazamientoPrimerAlbum = -1;
        desplazamientoPrimerMensaje = -1;
    }

    ~Usuario() {

    }
    void establecerAmistad(unsigned char* arregloBytes, int usuarioFila, int usuarioColumna, int totalUsuarios);
    bool consultarAmistad(const unsigned char* arregloBytes, int usuarioFila, int usuarioColumna, int totalUsuarios);
    int contarAmigosEnComun(const unsigned char* arregloBytes, int usuarioUno, int usuarioDos, int totalUsuarios);
    void sugerirAmistades(const unsigned char* arregloBytes, int usuarioActual, int totalUsuarios);
    uint getID() const { return ID; }
    const char* getNombreUsuario() const { return nombreUsuario; }
    const char* getContrasema() const { return contrasema; }
    long long getDesplazamientoPrimerAlbum() const { return desplazamientoPrimerAlbum; }
    long long getDesplazamientoPrimerMensaje() const { return desplazamientoPrimerMensaje; }

    void setID(unsigned int _id) { ID = _id; }
    void setDesplazamientoPrimerAlbum(long long desp) { desplazamientoPrimerAlbum = desp; }
    void setDesplazamientoPrimerMensaje(long long desp) { desplazamientoPrimerMensaje = desp; }
};
Usuario::Usuario() {
    ID = 0;
    copiarCadena(nombreUsuario," ",50);
    copiarCadena(contrasema," ",20);
    desplazamientoPrimerAlbum = -1;
    desplazamientoPrimerMensaje = -1;
}
void Usuario::establecerAmistad(unsigned char* arregloBytes, int usuarioActual, int usuarioAAgregar, int totalUsuarios) {
    int indiceLineal = usuarioActual * totalUsuarios + usuarioAAgregar;//
    int indiceByte = indiceLineal / 8;
    int indiceBit = indiceLineal % 8;
    arregloBytes[indiceByte] |= (1 << indiceBit);
}
bool Usuario::consultarAmistad(const unsigned char* arregloBytes, int usuarioFila, int usuarioColumna, int totalUsuarios) {
    int indiceLineal = usuarioFila * totalUsuarios + usuarioColumna;
    int indiceByte = indiceLineal / 8;
    int indiceBit = indiceLineal % 8;
    return (arregloBytes[indiceByte] & (1 << indiceBit)) != 0;
}
int Usuario::contarAmigosEnComun(const unsigned char* arregloBytes, int usuarioUno, int usuarioDos, int totalUsuarios) {
    int amigosEnComun = 0;
    int totalBytesPorFila = (totalUsuarios + 7) / 8;

    for (int indiceByteFila = 0; indiceByteFila < totalBytesPorFila; ++indiceByteFila) {
        int posicionByteUno = (usuarioUno * totalBytesPorFila) + indiceByteFila;
        int posicionByteDos = (usuarioDos * totalBytesPorFila) + indiceByteFila;

        // Operador AND a nivel de bits para encontrar intersecciones
        unsigned char interseccionAmistades = arregloBytes[posicionByteUno] & arregloBytes[posicionByteDos];

        // Contar los bits encendidos en este byte
        for (int indiceBit = 0; indiceBit < 8; ++indiceBit) {
            if ((interseccionAmistades & (1 << indiceBit)) != 0) {
                amigosEnComun++;
            }
        }
    }
    return amigosEnComun;
}
void Usuario::sugerirAmistades(const unsigned char* arregloBytes, int usuarioActual, int totalUsuarios) {
    cout << "\n--- Sugerencias de amistad para el usuario ID: " << usuarioActual << " ---" << endl;

    bool haySugerencias = false;

    for (int candidato = 0; candidato < totalUsuarios; ++candidato) {
        // Regla 1: Omitir al usuario actual
        if (candidato == usuarioActual) {
            continue;
        }

        // Regla 2: Omitir si ya son amigos directamente
        if (consultarAmistad(arregloBytes, usuarioActual, candidato, totalUsuarios)) {
            continue;
        }

        // Regla 3: Calcular amigos en comun
        int amigosEnComun = contarAmigosEnComun(arregloBytes, usuarioActual, candidato, totalUsuarios);

        // Si tienen al menos un amigo en comun, se considera una sugerencia valida
        if (amigosEnComun > 0) {
            cout << "-> Usuario sugerido: ID " << candidato
                 << " (Comparte " << amigosEnComun << " amigos en comun)" << endl;
            haySugerencias = true;
        }
    }

    if (!haySugerencias) {
        cout << "No hay sugerencias de amistad disponibles en este momento." << endl;
    }
}
#endif // USUARIO_H
