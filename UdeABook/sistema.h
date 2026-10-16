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

int calcularBytesMatriz(int n){
    return ((n*n)+7)/8;// Calcula el numero total de bytes necesarios para la matriz de bits (N * N bits / 8)
};
size_t calcularBytesMatriz(unsigned int n){
    return ((n*n)+7)/8;// Calcula el numero total de bytes necesarios para la matriz de bits (N * N bits / 8)
};

class Sistema {
private:
    Usuario* usuarioActual;
    uint cantidadDeUsuarios;
    bool sesionIniciada;
    char* rutaBaseUsuarios;
    char* rutaBaseMensajes;
    uchar* matrizAdyacente;
    char* usuarios;


public:
    Sistema(char* _rutaMatrizAdyacente, char* _rutaBaseUsuarios, char* _rutaBaseMensajes);
    ~Sistema();    // Destructor para liberar la memoria dinamica del nombre y de los flujos
    void calcularUsuariosDesdeDisco();
    bool abrirBaseDatosActual(char* rutaBaseUtilizada);
    void menuInicio();//
    bool iniciarSesion(char* idUsuario, char* contraseña);

    void menuPrincipal();
    void visitarPerfil(uint IdUsuario);
    bool enviarSolicitudDeAmistad(uint idUsuario,uint usuarioAEnviar);
    void cerrarBaseDatos();
    void registrarNuevoUsuario(const char* nombreUsuario, const char* contrasena) ;
    void registrarNuevoUsuarioEnMatriz(uchar*& matrizAdyacente);  // Registra un nuevo usuario, expandiendo dinamicamente la matriz de adyacencia en disco
    void agregarAmistad(uint id1, uint id2);// Establece amistad entre dos usuarios usando operaciones de bits
    void sugerirAmigos(uint idUsuario); // Sugiere amigos encontrando amigos en común mediante operaciones de bits
    bool estaSesionIniciada() const { return sesionIniciada; }
    uint getCantidadDeUsuarios() const { return cantidadDeUsuarios; }
    void calcularBytesDisco();

};

Sistema::Sistema(char* _rutaMatrizAdyacente, char* _rutaBaseUsuarios, char* _rutaBaseMensajes) {
    usuarioActual = nullptr;
    sesionIniciada = false;
    rutaBaseMensajes= _rutaBaseMensajes;
    rutaBaseUsuarios=_rutaBaseUsuarios;
    //cargar matriz;
    fstream archivoMatriz(_rutaMatrizAdyacente, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
    if (!archivoMatriz.is_open()) {
        ofstream creador("matrizAdyacente.txt", std::ios::binary);
        creador.close();
        archivoMatriz.open("matrizAdyacente.txt", std::ios::in | std::ios::out | std::ios::binary);
    }
    matrizAdyacente=new unsigned char[calcularBytesMatriz(cantidadDeUsuarios)];
    // Leer la matriz desde el archivo
    for (int usuarioFila = 0; usuarioFila < cantidadDeUsuarios; ++usuarioFila) {
        for (int usuarioColumna = 0; usuarioColumna < cantidadDeUsuarios; ++usuarioColumna) {
            int valorConexion;
            archivoMatriz >> valorConexion;
            if (valorConexion == 1) {
                int indiceLineal = usuarioFila * cantidadDeUsuarios + usuarioColumna;
                matrizAdyacente[indiceLineal / 8] |= (1 << (indiceLineal % 8));
            }
        }
    }
    archivoMatriz.close();

}
Sistema::~Sistema() {

}

void Sistema::registrarNuevoUsuario(const char* nombreUsuario, const char* contrasena) {
    // 1. Asignar el ID basandonos en la cantidad actual de usuarios (empezando desde 0)
    calcularUsuariosDesdeDisco();
    uint nuevoId = cantidadDeUsuarios+1;

    // 2. Crear el objeto Usuario en memoria temporal
    Usuario nuevoUsuario(nuevoId, nombreUsuario, contrasena);

    // 3. Abrir el archivo permanente de usuarios para escribir al final
    std::fstream archivoUsuarios(rutaBaseUsuarios, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
    if (!archivoUsuarios.is_open()) {
        std::ofstream creador("usuarios.txt", std::ios::binary);
        creador.close();
        archivoUsuarios.open("usuarios.txt", std::ios::in | std::ios::out | std::ios::binary);
    }

    // Obtener la posicion exacta en bytes donde se guardara este usuario
    archivoUsuarios.seekp(0, std::ios::end);
   // long long desplazamientoUsuario = archivoUsuarios.tellp();

    // Guardar el usuario en disco (Nota: Asegurate de tener un metodo escribir en la clase Usuario o serializarlo)
    // Para simplificar la escritura binaria de tamaño fijo de la clase Usuario:
    archivoUsuarios.write(reinterpret_cast<const char*>(&nuevoUsuario), sizeof(Usuario));
    archivoUsuarios.close();

    // 4. Ampliar la matriz de adyacencia de bits para el nuevo usuario
    registrarNuevoUsuarioEnMatriz(matrizAdyacente); // Esta función ya incrementa cantidadDeUsuarios y agranda la matriz en el archivo mat

    cout << "Usuario creado con éxito!\n";
    cout << " - ID: " << nuevoId << "\n";
    cout << " - Nombre: " << nombreUsuario << "\n";
}
void Sistema::registrarNuevoUsuarioEnMatriz(uchar*& matrizAdyacente) {
    cantidadDeUsuarios++;
    int nuevoTotalBits = cantidadDeUsuarios * cantidadDeUsuarios;
    int nuevoTotalBytes = (nuevoTotalBits + 7) / 8;

    // 1. Crear un nuevo arreglo dinamico mas grande inicializado en 0
    unsigned char* matrizNueva = new unsigned char[nuevoTotalBytes]();

    // 2. Copiar las conexiones existentes a su nueva posicion recalculada
    for (int fila = 0; fila < cantidadDeUsuarios-1; ++fila) {
        for (int columna = 0; columna < cantidadDeUsuarios-1; ++columna) {

            // Calcular el índice lineal en la matriz vieja
            int indiceAntiguo = fila * (cantidadDeUsuarios-1) + columna;
            bool tieneConexion = (matrizAdyacente[indiceAntiguo / 8] & (1 << (indiceAntiguo % 8))) != 0;

            if (tieneConexion) {
                // Calcular el nuevo indice lineal utilizando el nuevo ancho de la red
                int indiceNuevo = fila * cantidadDeUsuarios + columna;
                matrizNueva[indiceNuevo / 8] |= (1 << (indiceNuevo % 8));
            }
        }
    }

    // 3. Liberar la memoria de la matriz antigua
    delete[] matrizAdyacente;
    matrizAdyacente=matrizNueva;
     cout << "Nuevo usuario registrado. Total usuarios: " << cantidadDeUsuarios << "\n";

}



void Sistema::agregarAmistad(uint id1, uint id2) {
    if (id1 >= cantidadDeUsuarios || id2 >= cantidadDeUsuarios) return;
    usuarioActual->establecerAmistad(matrizAdyacente,id1, id2,cantidadDeUsuarios);
}
void Sistema::sugerirAmigos(uint idUsuario) {
    if (idUsuario >= cantidadDeUsuarios) return;
    usuarioActual->sugerirAmistades(matrizAdyacente,idUsuario,cantidadDeUsuarios);
}
void Sistema::menuInicio() {
    int opcion = 0;

    do {
        cout << "\n========================================\n";
        cout << "      Ude@Book - MENU DE INICIO       \n";
        cout << "========================================\n";
        cout << "1. Registrar nuevo usuario\n";
        cout << "2. Iniciar sesion\n";
        cout << "3. Salir\n";
        cout << "Seleccione una opcion: ";

        // Validación básica de entrada para evitar bucles infinitos si ingresan letras
        if (!(cin >> opcion)) {
            cin.clear(); // Limpiar el estado de error de cin
            cin.ignore(10000, '\n'); // Descartar la entrada inválida
            cout << "Error: Ingrese un numero valido.\n";
            continue;
        }

        // Limpiar el buffer de salto de línea pendiente
        cin.ignore(10000, '\n');

        switch (opcion) {
        case 1: {
            char nombre[50];
            char clave[20];

            cout << "\n--- REGISTRO DE NUEVO USUARIO ---\n";
            cout << "Ingrese nombre de usuario (max 50 chars): ";
            cin.getline(nombre, 50);

            cout << "Ingrese contrasena (max 20 chars): ";
            cin.getline(clave, 20);

            // Llamamos a la funcion que creamos previamente para registrar en disco
            registrarNuevoUsuario(nombre, clave);
            break;
        }
        case 2: {
            char nombre[50];
            char clave[20];

            cout << "\n--- INICIAR SESION ---\n";
            cout << "Usuario: ";
            cin.getline(nombre, 50);

            cout << "Contrasena: ";
            cin.getline(clave, 20);

            // Aquí implementaremos la lógica para verificar credenciales en 'usuarios.dat'
            // por ejemplo: iniciarSesion(nombre, clave);
            cout << "Funcion de inicio de sesion en proceso de conexion con el disco...\n";
            break;
        }
        case 3:
            cout << "\nSaliendo del sistema. Hasta pronto!\n";
            break;
        default:
            cout << "\nOpcion invalida. Intente de nuevo.\n";
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
void Sistema::calcularBytesDisco() {

}
#endif // SISTEMA_H
