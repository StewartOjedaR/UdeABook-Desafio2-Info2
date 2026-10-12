#ifndef USUARIO_H
#define USUARIO_H
#include <iostream>
#include <fstream>
#include "album.h"
#include "mensaje.h"
using namespace std;
typedef unsigned int uint;

class Usuario{
private:
    uint ID;// Identificador único
    char* nombreUsuario;
    char* contrasena;
    Album* album;
    Mensaje* mensajes;
    // Control de bloques libres (Lista de libres)
    bool activo;
    long long siguienteLibre;
    long long primerAlbumOffset;
    // Relación jerárquica
};
#endif // USUARIO_H
