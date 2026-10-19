#ifndef SISTEMA_H
#define SISTEMA_H
#include <iostream>
#include <fstream>
#include "album.h"
#include "mensaje.h"
#include "usuario.h"
using namespace std;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long long ulong;
bool sonCadenasIguales(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return false;
        i++;
    }
    return s1[i] == s2[i];
}
int calcularBytesMatriz(int n){
    return ((n*n)+7)/8;// Calcula el numero total de bytes necesarios para la matriz de bits (N * N bits / 8)
};
size_t calcularBytesMatriz(unsigned int n){
    return ((n*n)+7)/8;// Calcula el numero total de bytes necesarios para la matriz de bits (N * N bits / 8)
};

class Sistema {
private:
    Usuario* usuarioActual;
    uint contadorDeUsuarios;
    ulong contadorDeMensajesGlobales;                 // Identificador unico global del mensaje para generar IDs unicos globales
    bool sesionIniciada;
    char* rutaBaseUsuarios;
    char* rutaBaseMensajes;
    char* rutaBaseFotos;
    char* rutaMatrizAdyacente;
    uchar* matrizAdyacente;
    char* usuarios;


public:
    Sistema(char* _rutaMatrizAdyacente, char* _rutaBaseUsuarios, char* _rutaBaseMensajes, char* _rutaBaseFotos);

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
    bool getSesionIniciada() const { return sesionIniciada; }
    uint getContadorDeUsuarios() const { return contadorDeUsuarios; }
    uint contarMensajesTotalesEnDisco();
    Album* listarAlbumesAmigo(const char* rutaArchivoDisco, const char* nombreAmigoBuscado, int& totalEncontrados);


};

Sistema::Sistema(char* _rutaMatrizAdyacente, char* _rutaBaseUsuarios, char* _rutaBaseMensajes,char* _rutaBaseFotos) {
    usuarioActual = nullptr;
    sesionIniciada = false;
    rutaBaseMensajes= _rutaBaseMensajes;
    rutaBaseUsuarios=_rutaBaseUsuarios;
    rutaBaseFotos=_rutaBaseFotos;
    //cargar matriz;
    fstream archivoMatriz(_rutaMatrizAdyacente, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
    if (!archivoMatriz.is_open()) {
        ofstream creador("matrizAdyacente.txt", std::ios::binary);
        creador.close();
        archivoMatriz.open("matrizAdyacente.txt", std::ios::in | std::ios::out | std::ios::binary);
    }
    calcularUsuariosDesdeDisco();
    matrizAdyacente=new unsigned char[contadorDeUsuarios];
    // Leer la matriz desde el archivo
    for (int usuarioFila = 0; usuarioFila < contadorDeUsuarios; ++usuarioFila) {
        for (int usuarioColumna = 0; usuarioColumna < contadorDeUsuarios; ++usuarioColumna) {
            int valorConexion;
            archivoMatriz >> valorConexion;
            if (valorConexion == 1) {
                int indiceLineal = usuarioFila * contadorDeUsuarios + usuarioColumna;
                matrizAdyacente[indiceLineal / 8] |= (1 << (indiceLineal % 8));
            }
        }
    }
    archivoMatriz.close();
    contadorDeMensajesGlobales=contarMensajesTotalesEnDisco();
    rutaMatrizAdyacente=_rutaMatrizAdyacente;


}
Sistema::~Sistema() {

}
bool Sistema::iniciarSesion(char* nombreUsuario, char* contraseña){
    std::fstream archivoUsuarios(rutaBaseUsuarios, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
    if (!archivoUsuarios.is_open()) {
        std::ofstream creador("usuarios.txt", std::ios::binary);
        creador.close();
        archivoUsuarios.open("usuarios.txt", std::ios::in | std::ios::out | std::ios::binary);
    }
    Usuario* usuarioTemp= new Usuario(nombreUsuario, archivoUsuarios,usuarioActual);
    delete usuarioTemp;
    if (sonCadenasIguales(usuarioActual->getContrasema(),contraseña)){
        return true;
    }else{
        return false;
    }


}

void Sistema::registrarNuevoUsuario(const char* nombreUsuario, const char* contrasena) {
    // 1. Asignar el ID basandonos en la cantidad actual de usuarios (empezando desde 0)
    calcularUsuariosDesdeDisco();
    uint nuevoId = contadorDeUsuarios+1;

    // 2. Crear el objeto Usuario en memoria temporal
    Usuario nuevoUsuario(nuevoId, nombreUsuario,contrasena);

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
    contadorDeUsuarios++;
    int nuevoTotalBits = contadorDeUsuarios * contadorDeUsuarios;
    int nuevoTotalBytes = (nuevoTotalBits + 7) / 8;

    // 1. Crear un nuevo arreglo dinamico mas grande inicializado en 0
    unsigned char* matrizNueva = new unsigned char[nuevoTotalBytes]();

    // 2. Copiar las conexiones existentes a su nueva posicion recalculada
    for (int fila = 0; fila < contadorDeUsuarios-1; ++fila) {
        for (int columna = 0; columna < contadorDeUsuarios-1; ++columna) {

            // Calcular el índice lineal en la matriz vieja
            int indiceAntiguo = fila * (contadorDeUsuarios-1) + columna;
            bool tieneConexion = (matrizAdyacente[indiceAntiguo / 8] & (1 << (indiceAntiguo % 8))) != 0;

            if (tieneConexion) {
                // Calcular el nuevo indice lineal utilizando el nuevo ancho de la red
                int indiceNuevo = fila * contadorDeUsuarios + columna;
                matrizNueva[indiceNuevo / 8] |= (1 << (indiceNuevo % 8));
            }
        }
    }

    // 3. Liberar la memoria de la matriz antigua
    delete[] matrizAdyacente;
    matrizAdyacente=matrizNueva;
     cout << "Nuevo usuario registrado. Total usuarios: " << contadorDeUsuarios << "\n";

}
void Sistema::agregarAmistad(uint id1, uint id2) {
    if (id1 >= contadorDeUsuarios || id2 >= contadorDeUsuarios) return;
    usuarioActual->establecerAmistad(matrizAdyacente,id1, id2,contadorDeUsuarios);
}
void Sistema::sugerirAmigos(uint idUsuario) {
    if (idUsuario >= contadorDeUsuarios) return;
    usuarioActual->sugerirAmistades(matrizAdyacente,idUsuario,contadorDeUsuarios);
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
            int contador=0;

            while(!iniciarSesion(nombre, clave)&&contador<3 ){

                cout << "Iniciando sesion...\n"<<"Usuario o contreseña incorrecto, intente de nuevo";
                cout << "\n--- INICIAR SESION ---\n";
                cout << "Usuario: ";
                cin.getline(nombre, 50);
                cout << "Contrasena: ";
                cin.getline(clave, 20);
                contador++;
                if (contador==2){
                    sesionIniciada=false;
                }

            }
            if (!sesionIniciada){
                cout<<"Demasiados intentos, no se pudo iniciar sesion";
                break;
            }
            cout << "Iniciando sesion...\n";
            cout << "sesion iniciada";
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
        contadorDeUsuarios = 0;
        return;
    }

    // 'std::ios::ate' posiciona el cursor al final del archivo inmediatamente.
    // 'tellg()' nos da el tamaño total exacto del archivo en bytes.
    long long tamanioTotalBytes = archivo.tellg();
    archivo.close();

    if (tamanioTotalBytes <= 0) {
        contadorDeUsuarios = 0;
    } else {
        // Dividimos el peso total del archivo entre lo que pesa un solo objeto Usuario en bytes
        contadorDeUsuarios = static_cast<unsigned int>(tamanioTotalBytes / sizeof(Usuario));
    }
}
uint Sistema::contarMensajesTotalesEnDisco() {
    // Abrir el archivo en modo binario y colocar el cabezal directamente al final (ios::ate)
    ifstream archivo(rutaBaseMensajes, ios::binary | ios::ate);
    if (!archivo.is_open()) {
        // Si el archivo no existe o no se puede abrir, el total de mensajes es 0
        return 0;
    }

    // Obtener la posición actual del cabezal al final del archivo (nos da el tamaño total en bytes)
    long long bytesTotales = archivo.tellg();
    archivo.close();

    // Si el archivo está vacío, retorna 0
    if (bytesTotales <= 0) {
        return 0;
    }

    // Dividir los bytes totales entre el tamaño en bytes de una sola estructura MensajeD
    int totalMensajes = bytesTotales / sizeof(Mensaje);

    return totalMensajes;
}
void Sistema::menuPrincipal(){
    Registro* listaDeAmigos=nullptr;
    usuarioActual->listarAmigos(sesionIniciada,rutaMatrizAdyacente,rutaBaseUsuarios,contadorDeUsuarios,listaDeAmigos);


}



Album* Sistema::listarAlbumesAmigo(const char* rutaArchivoDisco, const char* nombreAmigoBuscado, int& totalEncontrados) {
    std::fstream archivo(rutaArchivoDisco, std::ios::in | std::ios::binary);
    if (!archivo.is_open()) {
        totalEncontrados = 0;
        return nullptr;
    }

    totalEncontrados = 0;
    Album temp;

    // Primera pasada: Contar registros que coincidan con el usuario y tengan privacidad permitida
    while (temp.leerDeArchivo(archivo)) {
        // Verificamos si la ruta contiene el nombre del usuario o si se evalúa la privacidad
        // Nota: Asumimos que la privacidad se almacena como "publico" o "amigos"
        bool esPublicoOAmigos = sonCadenasIguales(temp.getPrivacidad(), "publico") ||
                                sonCadenasIguales(temp.getPrivacidad(), "amigos");

        // Puedes validar adicionalmente si el archivo pertenece al usuario mediante su ruta o ID
        if (esPublicoOAmigos) {
            totalEncontrados++;
        }
    }

    if (totalEncontrados == 0) {
        archivo.close();
        return nullptr;
    }

    Album* listaFiltrada = new Album[totalEncontrados];
    archivo.clear();
    archivo.seekg(0, std::ios::beg);

    int idx = 0;
    while (temp.leerDeArchivo(archivo) && idx < totalEncontrados) {
        bool esPublicoOAmigos = sonCadenasIguales(temp.getPrivacidad(), "publico") ||
                                sonCadenasIguales(temp.getPrivacidad(), "amigos");
        if (esPublicoOAmigos) {
            listaFiltrada[idx] = temp;
            idx++;
        }
    }

    archivo.close();
    return listaFiltrada;
}
void iniciarCarruselAlbum(Album* fotosAlbum, int totalFotos, const char* rutaArchivoDisco) {
    if (fotosAlbum == nullptr || totalFotos <= 0) {
        std::cout << "No hay fotos para mostrar en el carrusel.\n";
        return;
    }

    int indiceActual = 0;
    int opcion = 0;

    while (true) {
        // Visualización de la foto actual
        std::cout << "\n========================================\n";
        std::cout << "              FOTOS          \n";
        std::cout << "========================================\n";
        std::cout << "Foto [" << (indiceActual + 1) << " de " << totalFotos << "]\n";
        std::cout << "ID Foto: " << fotosAlbum[indiceActual].getIdFoto() << "\n";
        std::cout << "Ruta: " << fotosAlbum[indiceActual].getRutaArchivo() << "\n";
        std::cout << "Likes: " << fotosAlbum[indiceActual].getConteoDeLikes() << "\n";
        std::cout << "----------------------------------------\n";
        std::cout << "Opciones:\n";
        std::cout << "1. Siguiente foto\n";
        std::cout << "2. Foto previa\n";
        std::cout << "3. Dar / Quitar like\n";
        std::cout << "4. Salir del album\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        if (opcion == 4) {
            std::cout << "Saliendo del album...\n";
            break;
        }

        switch (opcion) {
        case 1: // Siguiente (Circular)
            indiceActual = (indiceActual + 1) % totalFotos;
            break;
        case 2: // Previa (Circular)
            indiceActual = (indiceActual - 1 + totalFotos) % totalFotos;
            break;
        case 3: { // Dar / Quitar like
            // Modificamos el conteo de likes en memoria (ejemplo alternando +1 / -1 o con lógica de estado)
            // Para simplificar, incrementamos y actualizamos en disco.
            int likesActuales = fotosAlbum[indiceActual].getConteoDeLikes();
            fotosAlbum[indiceActual].setConteoDeLikes(likesActuales + 1); // O alternar lógica de like

            std::cout << "¡Accion de like procesada! Total de likes: " << fotosAlbum[indiceActual].getConteoDeLikes() << "\n";

            // Actualizar el cambio de manera persistente en el archivo binario
            // (Buscar el registro por idFoto y reescribir su bloque en el archivo)
            // ...
            break;
        }
        default:
            std::cout << "Opcion no valida. Intente de nuevo.\n";
            break;
        }
    }
}
#endif // SISTEMA_H
