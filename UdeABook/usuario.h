#ifndef USUARIO_H
#define USUARIO_H
#include <iostream>
#include <fstream>
using namespace std;
typedef unsigned int uint;
typedef unsigned short int ushort ;
bool cadenasIguales(const char* str1, const char* str2) {
    if (str1 == nullptr || str2 == nullptr) {
        return str1 == str2; // Manejo de punteros nulos por seguridad
    }

    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return false; // Si un carácter difiere, no son iguales
        }
        i++;
    }

    // Ambas deben terminar al mismo tiempo (mismo carácter nulo '\0')
    return str1[i] == str2[i];
}
struct Registro {
    int id;
    char nombre[50]; // Arreglo de caracteres fijo para almacenar el texto de cada nodo
};

struct ListaEnlazada {
    int id;
    char* nombreUsuario;
    ListaEnlazada* siguiente;

    // 1. Constructor para inicializar un nodo
    ListaEnlazada(int _id, const char* _nombreUsuario) {
        id = _id;
        nombreUsuario = new char[50];

        int i = 0;
        while (_nombreUsuario[i] != '\0' && i < 49) {
            nombreUsuario[i] = _nombreUsuario[i];
            i++;
        }
        nombreUsuario[i] = '\0';
        siguiente = nullptr;
    }

    // 2. Destructor para liberar la memoria en cadena de la lista
    ~ListaEnlazada() {
        delete[] nombreUsuario;
        delete siguiente;
    }

    void insertar(ListaEnlazada*& cabeza, int _id, const char* _nombreUsuario) {
        ListaEnlazada* nuevoNodo = new ListaEnlazada(_id, _nombreUsuario);

        // Caso 1: Si la lista está vacía, el nuevo nodo se convierte en la cabeza
        if (cabeza == nullptr) {
            cabeza = nuevoNodo;
        } else {
            // Caso 2: Si ya tiene elementos, recorremos hasta el final
            ListaEnlazada* actual = cabeza;
            while (actual->siguiente != nullptr) {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevoNodo;
        }
    }

    int contarNodos() const {
        int contador = 0;
        const ListaEnlazada* actual = this;
        while (actual != nullptr) {
            contador++;
            actual = actual->siguiente;
        }
        return contador;
    }

    Registro* copiarAArreglo(int& tamano) const {
        tamano = contarNodos();
        if (tamano == 0) return nullptr;

        // Reservamos memoria dinámica para el arreglo de registros
        Registro* arreglo = new Registro[tamano];

        const ListaEnlazada* actual = this;
        int i = 0;
        while (actual != nullptr) {
            // Copiamos el ID
            arreglo[i].id = actual->id;

            // Copiamos el texto carácter por carácter al arreglo
            int j = 0;
            while (actual->nombreUsuario[j] != '\0' && j < 49) {
                arreglo[i].nombre[j] = actual->nombreUsuario[j];
                j++;
            }
            arreglo[i].nombre[j] = '\0'; // Terminador nulo

            actual = actual->siguiente;
            i++;
        }

        return arreglo;
    }

};


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
    bool esActivo;
    long long desplazamientoPrimerAlbum;
    long long desplazamientoPrimerMensaje;


public:

    Usuario();
    Usuario(char* _nombreUsuario, fstream &archivoUsuarios, Usuario*& usuarioActual);
    Usuario(uint _id, const char* _nombreUsuario, const char* _contrasema) {
        ID = _id;
        copiarCadena(nombreUsuario,_nombreUsuario,50);
        copiarCadena(contrasema,_contrasema,20);
        desplazamientoPrimerAlbum = -1;
        desplazamientoPrimerMensaje = -1;
    }

    ~Usuario() {

    }
    void copiarCadena(char* destino, const char* origen, int maxLen) {
        int i = 0;
        while (origen[i] != '\0' && i < maxLen - 1) {
            destino[i] = origen[i];
            i++;
        }
        destino[i] = '\0';
    }
    void setEsActivo(bool estado){ esActivo=estado;}
    bool getEsActivo(){return esActivo;}
    void establecerAmistad(unsigned char* arregloBytes, int usuarioFila, int usuarioColumna, int totalUsuarios);
    bool consultarAmistad(const unsigned char* arregloBytes, int usuarioFila, int usuarioColumna, int totalUsuarios);
    int contarAmigosEnComun(const unsigned char* arregloBytes, int usuarioUno, int usuarioDos, int totalUsuarios);
    void sugerirAmistades(const unsigned char* arregloBytes, int usuarioActual, int totalUsuarios);
    void listarAmigos(bool sesionIniciada, char* nombreArchivoMatriz, char* nombreArchivoUsuarios, int cantidadDeUsuarios, Registro*& listaAmigos);
    uint getID() const { return ID; }
    const char* getNombreUsuario() const { return nombreUsuario; }
    const char* getContrasema() const { return contrasema; }
    long long getDesplazamientoPrimerAlbum() const { return desplazamientoPrimerAlbum; }
    long long getDesplazamientoPrimerMensaje() const { return desplazamientoPrimerMensaje; }

    void setID(unsigned int _id) { ID = _id; }
    void setDesplazamientoPrimerAlbum(long long desp) { desplazamientoPrimerAlbum = desp; }
    void setDesplazamientoPrimerMensaje(long long desp) { desplazamientoPrimerMensaje = desp; }
    // Guarda el objeto Usuario en el archivo binario.
    void escribirEnArchivo(std::fstream& archivo) const {
        archivo.write(reinterpret_cast<const char*>(this), sizeof(Usuario));
    }
    // Lee los datos del usuario desde el archivo binario.
    bool leerDeArchivo(std::fstream& archivo) {
        return archivo.read(reinterpret_cast<char*>(this), sizeof(Usuario)).good();
    }
};

Usuario::Usuario() {
    ID = 0;
    copiarCadena(nombreUsuario," ",50);
    copiarCadena(contrasema," ",20);
    desplazamientoPrimerAlbum = -1;
    desplazamientoPrimerMensaje = -1;
}
Usuario::Usuario(char* _nombreUsuario,fstream& archivoUsuarios, Usuario*& usuarioActual ){
    Usuario* usrTemp=nullptr;
    while (usrTemp->leerDeArchivo(archivoUsuarios)) {
        if (usrTemp->getEsActivo()) {
            // Comparamos cadenas manualmente para evitar std::string
            int i = 0;
            bool iguales = true;
            while (_nombreUsuario[i] != '\0' && usrTemp->getNombreUsuario()[i] != '\0') {
                if (_nombreUsuario[i] != usrTemp->getNombreUsuario()[i]) {
                    iguales = false;
                    break;
                }
                i++;
            }
            if (iguales && _nombreUsuario[i] == '\0' && usrTemp->getNombreUsuario()[i] == '\0') {
                usuarioActual = usrTemp; // Copiamos el objeto encontrado
                archivoUsuarios.close();
            }
        }
    }

    archivoUsuarios.close();
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
void Usuario::listarAmigos(bool sesionIniciada, char* nombreArchivoMatriz, char* nombreArchivoUsuarios, int cantidadDeUsuarios, Registro*& listarAmigo) {
    if (!sesionIniciada) {
        std::cout << "[Error] No hay ninguna sesion iniciada.\n";
        return;
    }

    unsigned int idUsuarioActual = getID();

    // Abrir ambos archivos binarios necesarios
    std::ifstream archivoMatriz(nombreArchivoMatriz, std::ios::binary);
    std::ifstream archivoUsuarios(nombreArchivoUsuarios, std::ios::binary);

    if (!archivoMatriz.is_open() || !archivoUsuarios.is_open()) {
        std::cout << "[Error] No se pudieron abrir los archivos de datos.\n";
        if (archivoMatriz.is_open()) archivoMatriz.close();
        if (archivoUsuarios.is_open()) archivoUsuarios.close();
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << "           LISTA DE MIS AMIGOS          \n";
    std::cout << "========================================\n";
    ListaEnlazada* lista=nullptr;
    bool tieneAmigos = false;
    int contadorAmigos=0;
    // Recorremos todos los usuarios registrados para ver quiénes son amigos en la matriz
    for (unsigned int candidato = 0; candidato < cantidadDeUsuarios; ++candidato) {
        if (candidato == idUsuarioActual) continue; // Ignorarse a sí mismo

        // Calcular la posición exacta del bit en la matriz de adyacencia
        unsigned long long bitGlobal = static_cast<unsigned long long>(idUsuarioActual) * cantidadDeUsuarios + candidato;
        unsigned long long byteIdx = bitGlobal / 8;
        unsigned int bitOff = bitGlobal % 8;

        archivoMatriz.seekg(byteIdx, std::ios::beg);
        unsigned char byteActual = 0;

        if (archivoMatriz.read(reinterpret_cast<char*>(&byteActual), 1)) {
            // Verificar si el bit esta encendido (hay amistad)
            if ((byteActual & (1 << bitOff)) != 0) {

                // ACCESO DIRECTO O(1): Salto directo al registro del amigo en 'usuarios.dat'
                long long desplazamientoUsuario = static_cast<long long>(candidato) * sizeof(Usuario);
                archivoUsuarios.seekg(desplazamientoUsuario, std::ios::beg);

                Usuario amigoLeido;
                // (Nota: para usar ifstream en leerDeArchivo, asegúrate de que acepte std::istream& en lugar de std::fstream&)
                if (amigoLeido.leerDeArchivo(reinterpret_cast<std::fstream&>(archivoUsuarios))) {
                    if (amigoLeido.getEsActivo()) {
                        contadorAmigos++;
                        std::cout << " - [ID: " << amigoLeido.getID() << "] " << amigoLeido.getNombreUsuario() << "\n";
                        lista->insertar(lista, amigoLeido.getID(),amigoLeido.getNombreUsuario());
                        tieneAmigos = true;
                    }
                }
            }
        }
    }
    listarAmigo=lista->copiarAArreglo(contadorAmigos);
    delete lista;
    if (!tieneAmigos) {
        cout << "Aun no tienes amigos agregados en UdeABook\n";
    }
    cout << "========================================\n";

    archivoMatriz.close();
    archivoUsuarios.close();
}

#endif // USUARIO_H
