#ifndef SISTEMA_H
#define SISTEMA_H
#include <iostream>
#include <fstream>
//#include "album.h"
//#include "mensaje.h"
#include "usuario.h"
using namespace std;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long long ulong;


size_t calcularBytesMatriz(unsigned int n){
    return ((n*n)+7)/8;// Calcula el número total de bytes necesarios para la matriz de bits (N * N bits / 8)
};

class Sistema {
private:
    Usuario* usuarioActual;
    fstream* baseDatosUtilizada;
    uint cantidadDeUsuarios;
    bool sesionIniciada;
    char* rutaMatrizAdycente;
    char* rutaBaseUsuarios;
    char* rutaBaseMensajes;


public:
    Sistema(char* _rutaMatrizAdyacente, char* _rutaBaseUsuarios, char* _rutaBaseMensajes);
    ~Sistema();    // Destructor para liberar la memoria dinámica del nombre y de los flujos
    void calcularUsuariosDesdeDisco();
    bool abrirBaseDatosActual(char* rutaBaseUtilizada);
    void menuInicio();//
    bool iniciarSesion(char* idUsuario, char* contraseña);

    void menuPrincipal();
    void visitarPerfil(uint IdUsuario);
    bool enviarSolicitudDeAmistad(uint idUsuario,uint usuarioAEnviar);

    void cerrarBaseDatos();
    void registrarNuevoUsuario();  // Registra un nuevo usuario, expandiendo dinámicamente la matriz de adyacencia en disco
    void agregarAmistad(uint id1, uint id2);// Establece amistad entre dos usuarios usando operaciones de bits
    void sugerirAmigos(uint idUsuario); // Sugiere amigos encontrando amigos en común mediante operaciones de bits
    bool estaSesionIniciada() const { return sesionIniciada; }
    uint getCantidadDeUsuarios() const { return cantidadDeUsuarios; }
    void registrarNuevoUsuario(const char* nombreUsuario, const char* contrasena,char* rutaBaseUsuario) ;
    char* getRutaMatrizAdyacente()const {return rutaMatrizAdycente;}
};

Sistema::Sistema(char* _rutaMatrizAdyacente, char* _rutaBaseUsuarios, char* _rutaBaseMensajes) {
    usuarioActual = nullptr;
    sesionIniciada = false;
    baseDatosUtilizada = nullptr;
    rutaBaseMensajes= _rutaBaseMensajes;
    rutaBaseUsuarios=_rutaBaseUsuarios;
    rutaMatrizAdycente=_rutaMatrizAdyacente;

}
Sistema::~Sistema() {
    cerrarBaseDatos();
}
bool Sistema::abrirBaseDatosActual(char* rutaBaseUtilizada){
    cerrarBaseDatos();
    baseDatosUtilizada = new fstream(rutaBaseUtilizada, std::ios::in | std::ios::out | std::ios::binary);
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
void Sistema::registrarNuevoUsuario(const char* nombreUsuario, const char* contrasena,char* rutaBaseUsuario) {
    // 1. Asignar el ID basandonos en la cantidad actual de usuarios (empezando desde 0)
    uint nuevoId = cantidadDeUsuarios;

    // 2. Crear el objeto Usuario en memoria temporal
    Usuario nuevoUsuario(nuevoId, nombreUsuario, contrasena);

    // 3. Abrir el archivo permanente de usuarios para escribir al final
    std::fstream archivoUsuarios(rutaBaseUsuario, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
    if (!archivoUsuarios.is_open()) {
        std::ofstream creador("usuarios.dat", std::ios::binary);
        creador.close();
        archivoUsuarios.open("usuarios.dat", std::ios::in | std::ios::out | std::ios::binary);
    }

    // Obtener la posición exacta en bytes donde se guardará este usuario (opcional si usas ID fijo,
    // pero útil para verificar el desplazamiento)
    archivoUsuarios.seekp(0, std::ios::end);
    long long desplazamientoUsuario = archivoUsuarios.tellp();

    // Guardar el usuario en disco (Nota: Asegúrate de tener un método escribir en la clase Usuario o serializarlo)
    // Para simplificar la escritura binaria de tamaño fijo de la clase Usuario:
    // archivoUsuarios.write(reinterpret_cast<const char*>(&nuevoUsuario), sizeof(Usuario));
    archivoUsuarios.close();

    // 4. Ampliar la matriz de adyacencia de bits para el nuevo usuario
    registrarNuevoUsuario(); // Esta función ya incrementa cantidadDeUsuarios y agranda la matriz en el archivo mat

    cout << "Usuario creado con éxito!\n";
    cout << " - ID: " << nuevoId << "\n";
    cout << " - Nombre: " << nombreUsuario << "\n";
}
void Sistema::registrarNuevoUsuario() {
    cantidadDeUsuarios++;

    ofstream archivo(rutaMatrizAdycente, std::ios::in | std::ios::out | std::ios::binary);
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

    fstream archivo(rutaMatrizAdycente, std::ios::in | std::ios::out | std::ios::binary);
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

    ifstream archivo(rutaMatrizAdycente, std::ios::binary);
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
            cout << " - Usuario " << candidato << " (" << amigosEnComun << " amigos en comun)\n";
        }
    }
    archivo.close();
}
void Sistema::menuInicio() {
    int opcion = 0;

    do {
        std::cout << "\n========================================\n";
        std::cout << "      Ude@Book - MENU DE INICIO       \n";
        std::cout << "========================================\n";
        std::cout << "1. Registrar nuevo usuario\n";
        std::cout << "2. Iniciar sesion\n";
        std::cout << "3. Salir\n";
        std::cout << "Seleccione una opcion: ";

        // Validación básica de entrada para evitar bucles infinitos si ingresan letras
        if (!(std::cin >> opcion)) {
            std::cin.clear(); // Limpiar el estado de error de cin
            std::cin.ignore(10000, '\n'); // Descartar la entrada inválida
            std::cout << "Error: Ingrese un numero valido.\n";
            continue;
        }

        // Limpiar el buffer de salto de línea pendiente
        std::cin.ignore(10000, '\n');

        switch (opcion) {
        case 1: {
            char nombre[32];
            char clave[32];

            std::cout << "\n--- REGISTRO DE NUEVO USUARIO ---\n";
            std::cout << "Ingrese nombre de usuario (max 50 chars): ";
            std::cin.getline(nombre, 32);

            std::cout << "Ingrese contrasena (max 20 chars): ";
            std::cin.getline(clave, 32);

            // Llamamos a la función que creamos previamente para registrar en disco
            registrarNuevoUsuario(nombre, clave,rutaBaseUsuarios);
            break;
        }
        case 2: {
            char nombre[32];
            char clave[32];

            std::cout << "\n--- INICIAR SESION ---\n";
            std::cout << "Usuario: ";
            std::cin.getline(nombre, 32);

            std::cout << "Contrasena: ";
            std::cin.getline(clave, 32);

            // Aquí implementaremos la lógica para verificar credenciales en 'usuarios.dat'
            // por ejemplo: iniciarSesion(nombre, clave);
            std::cout << "Funcion de inicio de sesion en proceso de conexion con el disco...\n";
            break;
        }
        case 3:
            std::cout << "\nSaliendo del sistema. Hasta pronto!\n";
            break;
        default:
            std::cout << "\nOpcion invalida. Intente de nuevo.\n";
        }

    } while (opcion != 3);
}
void Sistema::calcularUsuariosDesdeDisco() {
    std::ifstream archivo(rutaBaseUsuarios, std::ios::binary | std::ios::ate);
    if (!archivo.is_open()) {
        cantidadDeUsuarios = 0;
        return;
    }

    // 'std::ios::ate' posiciona el cursor al final del archivo inmediatamente.
    // 'tellg()' nos da el tamaño total exacto del archivo en bytes.
    long long tamanioTotalBytes = archivo.tellg();
    archivo.close();

    if (tamanioTotalBytes <= 0) {
        cantidadDeUsuarios = 0;
    } else {
        // Dividimos el peso total del archivo entre lo que pesa un solo objeto Usuario en bytes
        cantidadDeUsuarios = static_cast<unsigned int>(tamanioTotalBytes / sizeof(Usuario));
    }
}
#endif // SISTEMA_H
