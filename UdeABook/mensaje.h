#ifndef MENSAJE_H
#define MENSAJE_H
#include <iostream>
#include <fstream>
using namespace std;
typedef unsigned int uint;
typedef unsigned short int ushort;
typedef unsigned long long ulong;

struct Fecha {
    ushort anio;
    ushort mes;
    ushort dia;
    ushort hora;
    ushort minuto;

    bool esAnterior(const Fecha& otra) const {
        if (anio != otra.anio) return anio < otra.anio;
        if (mes != otra.mes) return mes < otra.mes;
        if (dia != otra.dia) return dia < otra.dia;
        if (hora != otra.hora) return hora < otra.hora;
        return minuto < otra.minuto;
    }

    bool esPosterioOIgual(const Fecha& otra) const {
        return !esAnterior(otra);
    }

    bool esAnteriorOIgual(const Fecha& otra) const {return esAnterior(otra) || (anio == otra.anio && mes == otra.mes && dia == otra.dia && hora == otra.hora && minuto == otra.minuto);
};
};
struct ReferenciaMensaje {
    long long desplazamientoEnDisco;
    Fecha fechaMensaje;
};
class Mensaje {
private:
    ulong idMensaje;                 // Identificador unico global del mensaje
    uint idRemitente;                     // ID del usuario que envia
    uint idDestinatario;                  // ID del usuario que recibe (el dueño de la bandeja o el contacto)
    Fecha fecha;                                  // Fecha y hora del mensaje
    char contenido[512];                          // Texto del mensaje (ampliado a 512 bytes)
    bool leido;                                   // Estado (true si ya fue leido)
    long long desplazamientoSiguienteMensaje;     // Desplazamiento en bytes al siguiente mensaje en el archivo (-1 si es el ultimo)
    void copiarCadena(char* destino, const char* origen, int maxLen) {
        int i = 0;
        while (origen[i] != '\0' && i < maxLen - 1) {
            destino[i] = origen[i];
            i++;
        }
        destino[i] = '\0';
    }


public:
    Mensaje();
    Mensaje(unsigned long long _idMsg, unsigned int _remitente, unsigned int _destinatario,const char* _contenido, Fecha _fecha);
    // Persistencia binaria directa del registro
    void escribirEnArchivo(std::ofstream& archivo) const {archivo.write(reinterpret_cast<const char*>(this), sizeof(Mensaje));}
    void escribirEnArchivo(std::fstream& archivo) const {archivo.write(reinterpret_cast<const char*>(this), sizeof(Mensaje));}
    bool leerDeArchivo(std::ifstream& archivo) {return archivo.read(reinterpret_cast<char*>(this), sizeof(Mensaje)).good();}
    bool leerDeArchivo(std::fstream& archivo) {return archivo.read(reinterpret_cast<char*>(this), sizeof(Mensaje)).good();}
    unsigned long long getIdMensaje() const { return idMensaje; }
    unsigned int getIdRemitente() const { return idRemitente; }
    unsigned int getIdDestinatario() const { return idDestinatario; }
    Fecha getFecha() const { return fecha; }
    const char* getContenido() const { return contenido; }
    bool estaLeido() const { return leido; }
    long long getDesplazamientoSiguienteMensaje() const { return desplazamientoSiguienteMensaje; }
    void setLeido(bool estado) { leido = estado; }
    void setDesplazamientoSiguienteMensaje(long long desplazamiento) { desplazamientoSiguienteMensaje = desplazamiento; }
    void enviarMensaje(uint idRemitente, uint idDestinatario, Fecha fechaEnvio, const char* texto,uint &contadorDeMensajesGlobales,char* rutaBaseMensajes) ;
    void leerNuevosMensajes(uint idUsuarioActual, char* rutaBaseMensajes);
    void buscarMensajesPorRangoFecha(uint idUsuarioActual, Fecha fechaInicio, Fecha fechaFin, char* rutaMensajes) const;
};
Mensaje::Mensaje() {
    idMensaje = 0;
    idRemitente = 0;
    idDestinatario = 0;
    fecha = {0, 0, 0, 0, 0};
    for (int i = 0; i < 512; ++i) contenido[i] = '\0';
    leido = false;
    desplazamientoSiguienteMensaje = -1;
}
Mensaje::Mensaje(unsigned long long _idMsg, unsigned int _remitente, unsigned int _destinatario,
                 const char* _contenido, Fecha _fecha) {
    idMensaje = _idMsg;
    idRemitente = _remitente;
    idDestinatario = _destinatario;
    fecha = _fecha;
    copiarCadena(contenido, _contenido, 512);
    leido = false;
    desplazamientoSiguienteMensaje = -1;
}
void Mensaje::enviarMensaje(uint idRemitente, uint idDestinatario, Fecha fechaEnvio, const char* texto,uint &contadorDeMensajesGlobales,char* rutaBaseMensajes) {

    Mensaje nuevoMensaje(++contadorDeMensajesGlobales, idRemitente, idDestinatario, texto, fechaEnvio);

    // Abrir el archivo en modo append (escritura al final) binario
    ofstream archivoSalida(rutaBaseMensajes, ios::out | ios::binary | ios::app);
    if (!archivoSalida.is_open()) {
        cout << "Error al abrir el archivo para enviar el mensaje." << endl;
        return;
    }

    archivoSalida.write(reinterpret_cast<const char*>(&nuevoMensaje), sizeof(Mensaje));
    archivoSalida.close();
    cout << "Mensaje enviado exitosamente al usuario ID " << idDestinatario << "!" << endl;
}
void Mensaje::leerNuevosMensajes(uint idUsuarioActual, char* rutaBaseMensajes) {
    fstream archivoBinario(rutaBaseMensajes, ios::in | ios::out | ios::binary);
    if (!archivoBinario.is_open()) {
        cout << "No se pudo abrir el archivo de mensajes." << endl;
        return;
    }

    Mensaje mensajeTemporal;
    int cantidadSinLeer = 0;

    // 1. Contar mensajes entrantes sin leer
    while (archivoBinario.read(reinterpret_cast<char*>(&mensajeTemporal), sizeof(Mensaje))) {
        if (mensajeTemporal.getIdDestinatario() == idUsuarioActual && !mensajeTemporal.estaLeido()) {
            cantidadSinLeer++;
        }
    }

    if (cantidadSinLeer == 0) {
        cout << "\nNo tienes mensajes entrantes sin leer." << endl;
        archivoBinario.close();
        return;
    }
    // 2. Crear arreglo temporal de referencias en el heap
    ReferenciaMensaje* arregloReferencias = new ReferenciaMensaje[cantidadSinLeer];
    archivoBinario.clear();
    archivoBinario.seekg(0, ios::beg);

    int indice = 0;
    while (true) {
        long long offset = archivoBinario.tellg();
        if (!archivoBinario.read(reinterpret_cast<char*>(&mensajeTemporal), sizeof(Mensaje))) break;

        if (mensajeTemporal.getIdDestinatario() == idUsuarioActual && !mensajeTemporal.estaLeido()) {
            if (indice < cantidadSinLeer) {
                arregloReferencias[indice].desplazamientoEnDisco = offset;
                arregloReferencias[indice].fechaMensaje = mensajeTemporal.getFecha();
                indice++;
            }
        }
    }

    // 3. Ordenar cronológicamente por fecha (Burbuja)
    for (int i = 0; i < cantidadSinLeer - 1; ++i) {
        for (int j = 0; j < cantidadSinLeer - i - 1; ++j) {
            if (!arregloReferencias[j].fechaMensaje.esAnteriorOIgual(arregloReferencias[j + 1].fechaMensaje)) {
                ReferenciaMensaje aux = arregloReferencias[j];
                arregloReferencias[j] = arregloReferencias[j + 1];
                arregloReferencias[j + 1] = aux;
            }
        }
    }

    // 4. Mostrar y actualizar estado a leido en el archivo original
    cout << "\n========================================" << endl;
    cout << "      NUEVOS MENSAJES RECIBIDOS         " << endl;
    cout << "========================================" << endl;

    for (int i = 0; i < cantidadSinLeer; ++i) {
        archivoBinario.seekg(arregloReferencias[i].desplazamientoEnDisco, ios::beg);
        archivoBinario.read(reinterpret_cast<char*>(&mensajeTemporal), sizeof(Mensaje));

        Fecha f = mensajeTemporal.getFecha();
        cout << "Remitente: " << mensajeTemporal.getIdRemitente() << endl
             << "Fecha: " << f.dia << "/" << f.mes << "/" << f.anio << " " << f.hora << ":" << f.minuto << endl
             << "Mensaje: \"" << mensajeTemporal.getContenido() << "\"" << endl;
        cout << "----------------------------------------" << endl;

        // Marcar como leido y reescribir en disco
        mensajeTemporal.setLeido(true);
        archivoBinario.seekp(arregloReferencias[i].desplazamientoEnDisco, ios::beg);
        archivoBinario.write(reinterpret_cast<const char*>(&mensajeTemporal), sizeof(Mensaje));
        archivoBinario.flush();
    }

    archivoBinario.close();
    delete[] arregloReferencias;
    cout << "Todos los nuevos mensajes han sido marcados como leidos!" << endl;
}

void Mensaje::buscarMensajesPorRangoFecha(uint idUsuarioActual, Fecha fechaInicio, Fecha fechaFin, char* rutaMensajes) const {
    ifstream archivoLectura(rutaMensajes, ios::binary);
    if (!archivoLectura.is_open()) {
        cout << "No se pudo abrir el archivo de mensajes." << endl;
        return;
    }

    Mensaje mensajeTemporal;
    int encontrados = 0;

    cout << "\n========================================" << endl;
    cout << "   BUSQUEDA DE MENSAJES POR FECHA       " << endl;
    cout << "========================================" << endl;

    while (archivoLectura.read(reinterpret_cast<char*>(&mensajeTemporal), sizeof(Mensaje))) {
        // Verificar si el usuario participa (remitente o destinatario)
        if (mensajeTemporal.getIdRemitente() == idUsuarioActual ||
            mensajeTemporal.getIdDestinatario() == idUsuarioActual) {

            Fecha f = mensajeTemporal.getFecha();
            // Comprobar si la fecha cae dentro del rango [fechaInicio, fechaFin]
            if (f.esPosterioOIgual(fechaInicio) && f.esAnteriorOIgual(fechaFin)) {
                encontrados++;
                bool esEnviado = (mensajeTemporal.getIdRemitente() == idUsuarioActual);

                cout << "[" << (esEnviado ? "ENVIADO A -> ID " : "RECIBIDO DE <- ID ")
                     << (esEnviado ? mensajeTemporal.getIdDestinatario() : mensajeTemporal.getIdRemitente()) << "]" << endl
                     << "Fecha: " << f.dia << "/" << f.mes << "/" << f.anio << " " << f.hora << ":" << f.minuto << endl
                     << "Mensaje: \"" << mensajeTemporal.getContenido() << "\"" << endl;
                cout << "----------------------------------------" << endl;
            }
        }
    }

    if (encontrados == 0) {
        cout << "No se encontraron mensajes en el rango de fechas especificado." << endl;
    }

    archivoLectura.close();
}
#endif // MENSAJE_H
