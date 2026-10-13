#ifndef ALBUM_H
#define ALBUM_H
#include <fstream>
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
class Foto {
private:
    unsigned long long idFoto;
    unsigned int idUsuario;
    unsigned int idAlbum;
    char rutaArchivo[256];
    long long desplazamientoSiguienteFoto;

public:
    Foto() {
        idFoto = 0;
        idUsuario = 0;
        idAlbum = 0;
        for (int i = 0; i < 256; ++i) rutaArchivo[i] = '\0';
        desplazamientoSiguienteFoto = -1;
    }

    Foto(unsigned long long _idFoto, unsigned int _idUsuario, unsigned int _idAlbum, const char* _ruta) {
        idFoto = _idFoto;
        idUsuario = _idUsuario;
        idAlbum = _idAlbum;
        copiarCadena(rutaArchivo, _ruta, 256);
        desplazamientoSiguienteFoto = -1;
    }

    void escribirEnArchivo(std::fstream& archivo) const {
        archivo.write(reinterpret_cast<const char*>(this), sizeof(Foto));
    }

    bool leerDeArchivo(std::fstream& archivo) {
        return archivo.read(reinterpret_cast<char*>(this), sizeof(Foto)).good();
    }

    unsigned long long getIdFoto() const { return idFoto; }
    unsigned int getIdUsuario() const { return idUsuario; }
    unsigned int getIdAlbum() const { return idAlbum; }
    const char* getRutaArchivo() const { return rutaArchivo; }
    long long getDesplazamientoSiguienteFoto() const { return desplazamientoSiguienteFoto; }

    void setDesplazamientoSiguienteFoto(long long desp) { desplazamientoSiguienteFoto = desp; }
};

/*class Album{
private:
    char* nombreAlbum;
    Fecha fecha;
    char* ruta;
    uint IDusuario;
    char* nivelPrivacidad;
};*/

class Album {
private:
    uint idAlbum;
    unsigned int idUsuario;
    char nombreAlbum[50];
    char privacidad[10];
    long long desplazamientoPrimerFoto;
    long long desplazamientoSiguienteAlbum;

public:
    Album() {
        idAlbum = 0;
        idUsuario = 0;
        for (int i = 0; i < 64; ++i) nombreAlbum[i] = '\0';
        copiarCadena(privacidad,"publico",10);
        desplazamientoPrimerFoto = -1;
        desplazamientoSiguienteAlbum = -1;
    }

    Album(unsigned int _idAlbum, unsigned int _idUsuario, const char* _nombre, char* _privado) {
        idAlbum = _idAlbum;
        idUsuario = _idUsuario;
        copiarCadena(nombreAlbum, _nombre, 64);
        copiarCadena(privacidad,_privado,10);
        desplazamientoPrimerFoto = -1;
        desplazamientoSiguienteAlbum = -1;
    }

    void escribirEnArchivo(std::fstream& archivo) const {
        archivo.write(reinterpret_cast<const char*>(this), sizeof(Album));
    }

    bool leerDeArchivo(std::fstream& archivo) {
        return archivo.read(reinterpret_cast<char*>(this), sizeof(Album)).good();
    }

    unsigned int getIdAlbum() const { return idAlbum; }
    unsigned int getIdUsuario() const { return idUsuario; }
    const char* getNombreAlbum() const { return nombreAlbum; }
  //  bool esPrivado() const { return privado; }
    long long getDesplazamientoPrimerFoto() const { return desplazamientoPrimerFoto; }
    long long getDesplazamientoSiguienteAlbum() const { return desplazamientoSiguienteAlbum; }

    void setDesplazamientoPrimerFoto(long long desp) { desplazamientoPrimerFoto = desp; }
    void setDesplazamientoSiguienteAlbum(long long desp) { desplazamientoSiguienteAlbum = desp; }
};
#endif // ALBUM_H
