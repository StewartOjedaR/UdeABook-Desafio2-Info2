#ifndef ALBUM_H
#define ALBUM_H

#include <fstream>
#include <filesystem>
#include <iostream>
#include "fecha.h"

namespace fs = std::filesystem;
using namespace std;

typedef unsigned int uint;
typedef unsigned short int ushort;
void copiarCadena(char* destino, const char* origen, int maxLen) {
    int i = 0;
    while (origen[i] != '\0' && i < maxLen - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}
class Album {
private:

    uint idAlbum;
    unsigned int idUsuario;
    char nombreAlbum[50];
    char privacidad[10];
    unsigned long long idFoto;
    char rutaArchivo[256];
    int tamañoEnKB;
    int conteoDeLikes;
    Fecha fechaCaptura;
    long long desplazamientoSiguiente;


public:

    Album() {
        idAlbum = 0;
        idUsuario = 0;
        for (int i = 0; i < 50; ++i) nombreAlbum[i] = '\0';
        copiarCadena(privacidad, "publico", 10);

        idFoto = 0;
        for (int i = 0; i < 256; ++i) rutaArchivo[i] = '\0';
        tamañoEnKB = 0;
        conteoDeLikes = 0;
        desplazamientoSiguiente = -1;
    }

    void escribirEnArchivo(std::fstream& archivo) const {
        archivo.write(reinterpret_cast<const char*>(this), sizeof(Album));
    }

    bool leerDeArchivo(std::fstream& archivo) {
        return archivo.read(reinterpret_cast<char*>(this), sizeof(Album)).good();
    }
    void registrarYGuardarFoto(char* nombreUsuario, char* nombreAlbumParam, char* nombreFoto,
                               unsigned int _idUsuario, uint _idAlbum, unsigned long long _idFoto,
                               std::fstream& archivoDisco) {
        idAlbum = _idAlbum;
        idUsuario = _idUsuario;
        idFoto = _idFoto;
        copiarCadena(nombreAlbum, nombreAlbumParam, 50);

        // Construcción de la ruta física usando un char[] y lógica manual (sin <string>)
        char rutaComun[] = "./users/udeabook";
        int i = 0;

        for (int j = 0; rutaComun[j] != '\0'; j++) {
            rutaArchivo[i++] = rutaComun[j];
        }
        rutaArchivo[i++] = '/';

        for (int j = 0; nombreUsuario[j] != '\0'; j++) {
            rutaArchivo[i++] = nombreUsuario[j];
        }

        const char* sufijoAlbums = "/albums/";
        for (int j = 0; sufijoAlbums[j] != '\0'; j++) {
            rutaArchivo[i++] = sufijoAlbums[j];
        }

        for (int j = 0; nombreFoto[j] != '\0'; j++) {
            rutaArchivo[i++] = nombreFoto[j];
        }
        rutaArchivo[i] = '\0';
        // 1. Crear directorios físicamente en el disco
        std::error_code ec;
        fs::create_directories(rutaArchivo, ec);
        if (ec) {
            std::cerr << "Error al crear directorios: " << ec.message() << '\n';
        }
        const char* extension = ".png";
        for (int j = 0; extension[j] != '\0'; j++) {
            rutaArchivo[i++] = extension[j];
        }
        rutaArchivo[i] = '\0';

        archivoDisco.seekp(0, std::ios::end);
        escribirEnArchivo(archivoDisco);
    }
    uint getIdAlbum() const { return idAlbum; }
    unsigned int getIdUsuario() const { return idUsuario; }
    const char* getNombreAlbum() const { return nombreAlbum; }
    unsigned long long getIdFoto() const { return idFoto; }
    const char* getRutaArchivo() const { return rutaArchivo; }
    char*  const getPrivacidad(){return privacidad;}
    int getConteoDeLikes(){return conteoDeLikes;}
    void setConteoDeLikes(int likes){
        conteoDeLikes=likes;
    }

};

#endif // ALBUM_H