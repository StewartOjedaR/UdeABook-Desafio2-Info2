#ifndef USUARIO_H
#define USUARIO_H

//using namespace std;
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
    bool activo;

public:
    Usuario() {
        ID = 0;
        copiarCadena(nombreUsuario," ",50);
        copiarCadena(contrasema," ",20);
        desplazamientoPrimerAlbum = -1;
        desplazamientoPrimerMensaje = -1;
        activo = false;
    }

    Usuario(uint _id, const char* _nombreUsuario, const char* _contrasema) {
        ID = _id;
        copiarCadena(nombreUsuario,_nombreUsuario,50);
        copiarCadena(contrasema,_contrasema,20);
        desplazamientoPrimerAlbum = -1;
        desplazamientoPrimerMensaje = -1;
        activo = true;
    }

    ~Usuario() {

    }

    uint getID() const { return ID; }
    const char* getNombreUsuario() const { return nombreUsuario; }
    const char* getContrasema() const { return contrasema; }
    bool esActivo() const { return activo; }
    long long getDesplazamientoPrimerAlbum() const { return desplazamientoPrimerAlbum; }
    long long getDesplazamientoPrimerMensaje() const { return desplazamientoPrimerMensaje; }

    void setID(unsigned int _id) { ID = _id; }
    void setActivo(bool _activo) { activo = _activo; }
    void setDesplazamientoPrimerAlbum(long long desp) { desplazamientoPrimerAlbum = desp; }
    void setDesplazamientoPrimerMensaje(long long desp) { desplazamientoPrimerMensaje = desp; }
};
#endif // USUARIO_H
