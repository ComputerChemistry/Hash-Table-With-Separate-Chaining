#include <iostream>
#include <string>
#include <limits>

#include "Cabeceras/Hasher.hpp"
#include "Cabeceras/TablaHash.hpp"


void probarHashers();

void probarMiTabla_InsercionRehashBusquedaEliminacion();

void probarTablaNumerica_ColisionesAgresivas();

void probarMetodosUtilitarios();

void modoInteractivo();


void mostrarMenuPrincipal() {
    std::cout << "\n\n===== MENU DE PRUEBAS TABLA HASH =====" << std::endl;
    std::cout << "1. Probar Funciones Hash (HasherString64, HasherInt64)" << std::endl;
    std::cout << "2. Pruebas Basicas y Redimensionamiento (miTabla - string,int)" << std::endl;
    std::cout << "3. Pruebas Agresivas de Colision/Rehash (tablaNumerica - int,double)" << std::endl;
    std::cout << "4. Probar Metodos Utilitarios (EstaVacia, Vaciar, FactorCarga)" << std::endl;
    std::cout << "5. MODO INTERACTIVO" << std::endl;
    std::cout << "6. Ejecutar TODAS las pruebas automatizadas" << std::endl;
    std::cout << "0. Salir" << std::endl;
    std::cout << "Seleccione una opcion: ";
}

void esperarEnter() {
    std::cout << "\nPresione Enter para continuar...";
    std::cin.get();
}

void limpiarConsola() {
    std::system("cls");
}


std::string obtenerLinea(const std::string &prompt) {
    std::string linea;
    std::cout << prompt;
    std::getline(std::cin, linea);
    return linea;
}

void mostrarSubMenuInteractivo() {
    std::cout << "\n--- Modo Interactivo ---" << std::endl;
    std::cout << "1. Insertar elemento (clave: string, valor: string)" << std::endl;
    std::cout << "2. Buscar elemento (por clave string)" << std::endl;
    std::cout << "3. Eliminar elemento (por clave string)" << std::endl;
    std::cout << "4. Mostrar tabla actual" << std::endl;
    std::cout << "5. Vaciar tabla actual" << std::endl;
    std::cout << "0. Volver al menu principal" << std::endl;
    std::cout << "Seleccione una opcion: ";
}


void modoInteractivo() {
    TablaHash<std::string, std::string, HasherString64> tablaInteractiva(5);
    int opcionSubMenu;
    std::string claveEntrada;
    std::string valorEntrada;
    std::string valorEncontrado;
    bool resultadoOperacion;

    std::cout << "--- INICIO: Modo Interactivo ---" << std::endl;
    std::cout << "Se ha creado una TablaHash<string, string> vacia." << std::endl;

    do {
        limpiarConsola();
        std::cout << "Estado Actual de la Tabla Interactiva:" << std::endl;
        tablaInteractiva.MostrarTabla();
        mostrarSubMenuInteractivo();

        std::cin >> opcionSubMenu;
        while (std::cin.fail()) {
            std::cout << "Entrada invalida. Por favor, ingrese un numero." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Seleccione una opcion: ";
            std::cin >> opcionSubMenu;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (opcionSubMenu) {
            case 1:
                claveEntrada = obtenerLinea("Ingrese la clave (string): ");
                valorEntrada = obtenerLinea("Ingrese el valor (string): ");
                resultadoOperacion = tablaInteractiva.Insertar(claveEntrada, valorEntrada);
                if (resultadoOperacion) {
                    std::cout << "Elemento (\"" << claveEntrada << "\", \"" << valorEntrada <<
                            "\") insertado exitosamente." << std::endl;
                } else {
                    std::cout << "Clave \"" << claveEntrada << "\" ya existia. Valor actualizado a \"" << valorEntrada
                            << "\"." << std::endl;
                }
                break;
            case 2:
                claveEntrada = obtenerLinea("Ingrese la clave a buscar: ");
                resultadoOperacion = tablaInteractiva.Buscar(claveEntrada, valorEncontrado);
                if (resultadoOperacion) {
                    std::cout << "Clave \"" << claveEntrada << "\" encontrada. Valor: \"" << valorEncontrado << "\"." <<
                            std::endl;
                } else {
                    std::cout << "Clave \"" << claveEntrada << "\" no encontrada." << std::endl;
                }
                break;
            case 3:
                claveEntrada = obtenerLinea("Ingrese la clave a eliminar: ");
                resultadoOperacion = tablaInteractiva.Eliminar(claveEntrada);
                if (resultadoOperacion) {
                    std::cout << "Clave \"" << claveEntrada << "\" eliminada exitosamente." << std::endl;
                } else {
                    std::cout << "Clave \"" << claveEntrada << "\" no encontrada, no se pudo eliminar." << std::endl;
                }
                break;
            case 4:
                std::cout << "La tabla se muestra al inicio de cada ciclo del sub-menu." << std::endl;
                break;
            case 5:
                tablaInteractiva.Vaciar();
                std::cout << "Tabla interactiva vaciada." << std::endl;
                break;
            case 0:
                std::cout << "Volviendo al menu principal..." << std::endl;
                break;
            default:
                std::cout << "Opcion no valida. Intente de nuevo." << std::endl;
                break;
        }
        if (opcionSubMenu != 0) {
            esperarEnter();
        }
    } while (opcionSubMenu != 0);
    std::cout << "--- FIN: Modo Interactivo ---" << std::endl;
}


int main() {
    int opcion;
    do {
        limpiarConsola();
        mostrarMenuPrincipal();
        std::cin >> opcion;

        while (std::cin.fail()) {
            std::cout << "Entrada invalida. Por favor, ingrese un numero." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Seleccione una opcion: ";
            std::cin >> opcion;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        limpiarConsola();

        switch (opcion) {
            case 1:
                probarHashers();
                break;
            case 2:
                probarMiTabla_InsercionRehashBusquedaEliminacion();
                break;
            case 3:
                probarTablaNumerica_ColisionesAgresivas();
                break;
            case 4:
                probarMetodosUtilitarios();
                break;
            case 5:
                modoInteractivo();
                break;
            case 6:
                std::cout << "--- Ejecutando TODAS las pruebas automatizadas ---" << std::endl;
                probarHashers();
                esperarEnter();
                limpiarConsola();
                probarMiTabla_InsercionRehashBusquedaEliminacion();
                esperarEnter();
                limpiarConsola();
                probarTablaNumerica_ColisionesAgresivas();
                esperarEnter();
                limpiarConsola();
                probarMetodosUtilitarios();
                std::cout << "\n--- TODAS las pruebas automatizadas finalizadas ---" << std::endl;
                break;
            case 0:
                std::cout << "Saliendo del programa de pruebas." << std::endl;
                break;
            default:
                std::cout << "Opcion no valida. Intente de nuevo." << std::endl;
                break;
        }

        if (opcion != 0) {
            esperarEnter();
        }
    } while (opcion != 0);

    return 0;
}


void probarHashers() {
    std::cout << "--- INICIO: Pruebas de Funciones Hash ---" << std::endl;
    HasherString64 stringHasher;
    const char *stringsPruebaArr[] = {
        "hola", "mundo", "ballena", "hashing", "", "Irene la Mejor Profe"
    };
    size_t numStrings = sizeof(stringsPruebaArr) / sizeof(stringsPruebaArr[0]);
    std::cout << "\nProbando HasherString64" << std::endl;
    for (size_t i = 0; i < numStrings; ++i) {
        uint64_t hashValor = stringHasher(stringsPruebaArr[i]);
        uint64_t hashValorConSemilla = stringHasher(stringsPruebaArr[i], 031001);
        std::cout << "Clave: \"" << stringsPruebaArr[i] << "\"" << std::endl;
        std::cout << " Hash (seed = 0): " << hashValor << std::endl;
        std::cout << " Hash (seed = " << 031001 << "): " << hashValorConSemilla << std::endl;
    }
    HasherInt64 intHasher;
    int intsPruebaArr[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    };
    size_t numInts = sizeof(intsPruebaArr) / sizeof(intsPruebaArr[0]);
    std::cout << "\nProbando HasherInt64" << std::endl;
    for (size_t i = 0; i < numInts; ++i) {
        int num = intsPruebaArr[i];
        uint64_t hashValor = intHasher(num);
        uint64_t hashValorConSemilla = intHasher(num, 031001);
        std::cout << "Clave: \"" << num << "\"" << std::endl;
        std::cout << " Hash (seed = 0): " << hashValor << std::endl;
        std::cout << " Hash (seed = " << 031001 << "): " << hashValorConSemilla << std::endl;
    }
    std::cout << "--- FIN: Pruebas de Funciones Hash ---" << std::endl;
}

void probarMiTabla_InsercionRehashBusquedaEliminacion() {
    std::cout << "--- INICIO: Pruebas Basicas y Redimensionamiento (miTabla - string,int) ---" << std::endl;
    std::cout << "\n Probando getters y estado inicial de miTabla" << std::endl;
    TablaHash<std::string, int, HasherString64> miTabla(4);
    std::cout << "\nInstancia de TablaHash::<std::string, int> creada (miTabla)." << std::endl;
    std::cout << "Capacidad inicial: 4, Capacidad obtenida: " << miTabla.ObtenerCapacidad() << std::endl;
    std::cout << "Numero de elementos inicial: 0, Numero de elementos obtenidos: " << miTabla.ObtenerNumElementos() <<
            std::endl;
    miTabla.MostrarTabla();

    std::cout << "\n Probando Insertar en miTabla (con posible rehash)" << std::endl;
    bool resultadoOperacion;
    const char *miTablaClaves[] = {"hola", "mundo", "adios", "prueba", "hello", "otra", "mas"};
    int miTablaValores[] = {100, 200, 300, 400, 500, 600, 700};
    size_t numMiTablaInserciones = sizeof(miTablaClaves) / sizeof(miTablaClaves[0]);

    for (size_t i = 0; i < numMiTablaInserciones; ++i) {
        resultadoOperacion = miTabla.Insertar(miTablaClaves[i], miTablaValores[i]);
        std::cout << "Insertando (\"" << miTablaClaves[i] << "\"," << miTablaValores[i] << "): "
                << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)")
                << ". Elementos: " << miTabla.ObtenerNumElementos()
                << ". Capacidad: " << miTabla.ObtenerCapacidad() << std::endl;
    }
    std::cout << "\n === ESTADO DE miTabla DESPUES DE INSERCIONES Y POSIBLES REHASHES ===" << std::endl;
    miTabla.MostrarTabla();

    resultadoOperacion = miTabla.Insertar("hola", 101);
    std::cout << "\nInsertando (\"hola\", 101) de nuevo: " << (resultadoOperacion
                                                                   ? "Exito (nuevo)"
                                                                   : "Fallo (ya existe/actualizado)")
            << ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;
    std::cout << "\n === ESTADO DE miTabla DESPUES DE ACTUALIZAR 'hola' ===" << std::endl;
    miTabla.MostrarTabla();

    std::cout << "\n Probando Buscar en miTabla (antes de eliminar)" << std::endl;
    int valorEncontradoInt;
    resultadoOperacion = miTabla.Buscar("hola", valorEncontradoInt);
    std::cout << "Buscando \"hola\": " << (resultadoOperacion ? "Encontrado" : "No encontrado") << (resultadoOperacion
            ? ", Valor: " + std::to_string(valorEncontradoInt)
            : "") << std::endl;
    resultadoOperacion = miTabla.Buscar("mundo", valorEncontradoInt);
    std::cout << "Buscando \"mundo\": " << (resultadoOperacion ? "Encontrado" : "No encontrado") << (resultadoOperacion
            ? ", Valor: " + std::to_string(valorEncontradoInt)
            : "") << std::endl;
    resultadoOperacion = miTabla.Buscar("adios", valorEncontradoInt);
    std::cout << "Buscando \"adios\": " << (resultadoOperacion ? "Encontrado" : "No encontrado") << (resultadoOperacion
            ? ", Valor: " + std::to_string(valorEncontradoInt)
            : "") << std::endl;
    resultadoOperacion = miTabla.Buscar("inexistente", valorEncontradoInt);
    std::cout << "Buscando \"inexistente\": " << (resultadoOperacion ? "Encontrado" : "No encontrado") << std::endl;

    std::cout << "\n Probando Eliminar de miTabla" << std::endl;
    miTabla.Eliminar("mundo");
    std::cout << "Eliminado \"mundo\". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;
    miTabla.Eliminar("hola");
    std::cout << "Eliminado \"hola\". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;
    std::cout << "\n === ESTADO DE miTabla DESPUES DE ALGUNAS ELIMINACIONES ===" << std::endl;
    miTabla.MostrarTabla();
    std::cout << "--- FIN: Pruebas Basicas y Redimensionamiento (miTabla) ---" << std::endl;
}

void probarTablaNumerica_ColisionesAgresivas() {
    std::cout << "--- INICIO: Pruebas Agresivas de Colision/Rehash (tablaNumerica - int,double) ---" << std::endl;
    bool resultadoOperacion;
    double valorEncontradoDouble;

    TablaHash<int, double, HasherInt64> tablaNumerica(2);
    std::cout << "\nEstado inicial tablaNumerica:" << std::endl;
    tablaNumerica.MostrarTabla();

    int clavesParaColision[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    double valorBase = 0.1;

    for (int clave: clavesParaColision) {
        resultadoOperacion = tablaNumerica.Insertar(clave, clave * valorBase);
        std::cout << "Insertando (" << clave << ", " << clave * valorBase << "): "
                << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)")
                << ". Elementos: " << tablaNumerica.ObtenerNumElementos()
                << ". Capacidad: " << tablaNumerica.ObtenerCapacidad() << std::endl;
        if (clave == 2 || clave == 5 || clave == 9) {
            std::cout << "\n === ESTADO DE tablaNumerica (intermedio) ===" << std::endl;
            tablaNumerica.MostrarTabla();
        }
    }

    std::cout << "\n === ESTADO FINAL DE tablaNumerica DESPUES DE INSERCIONES AGRESIVAS ===" << std::endl;
    tablaNumerica.MostrarTabla();

    std::cout << "\nProbando Buscar en tablaNumerica (con colisiones y rehashes)" << std::endl;
    for (int clave: clavesParaColision) {
        resultadoOperacion = tablaNumerica.Buscar(clave, valorEncontradoDouble);
        std::cout << "Buscando " << clave << ": " << (resultadoOperacion ? "Encontrado" : "No encontrado");
        if (resultadoOperacion) {
            std::cout << ", Valor: " << valorEncontradoDouble << ((valorEncontradoDouble == clave * valorBase)
                                                                      ? " (Correcto)"
                                                                      : " (INCORRECTO!)") << std::endl;
        } else {
            std::cout << " (ERROR: Deberia encontrarse!)" << std::endl;
        }
    }
    resultadoOperacion = tablaNumerica.Buscar(999, valorEncontradoDouble);
    std::cout << "Buscando 999: " << (resultadoOperacion ? "Encontrado" : "No encontrado") << std::endl;

    std::cout << "\nProbando Eliminar en tablaNumerica (con colisiones y rehashes)" << std::endl;
    int clavesParaEliminar[] = {5, 0, 10, 3, 7};
    for (int clave: clavesParaEliminar) {
        resultadoOperacion = tablaNumerica.Eliminar(clave);
        std::cout << "Eliminando " << clave << ": " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
                << ". Elementos: " << tablaNumerica.ObtenerNumElementos()
                << ". Capacidad: " << tablaNumerica.ObtenerCapacidad() << std::endl;
        resultadoOperacion = tablaNumerica.Buscar(clave, valorEncontradoDouble);
        std::cout << "Buscando " << clave << " post-eliminacion: " << (resultadoOperacion
                                                                           ? "Encontrado (ERROR!)"
                                                                           : "No encontrado (Correcto)") << std::endl;
    }
    std::cout << "\n === ESTADO FINAL DE tablaNumerica DESPUES DE ELIMINACIONES AGRESIVAS ===" << std::endl;
    tablaNumerica.MostrarTabla();
    std::cout << "--- FIN: Pruebas Agresivas de Colision/Rehash (tablaNumerica) ---" << std::endl;
}

void probarMetodosUtilitarios() {
    std::cout << "--- INICIO: Pruebas de Metodos Utilitarios ---" << std::endl;
    TablaHash<std::string, int, HasherString64> tablaUtil(5);

    std::cout << "\nEstado inicial tablaUtil:" << std::endl;
    std::cout << "  EstaVacia()=" << (tablaUtil.EstaVacia() ? "true" : "false")
            << ", Elementos=" << tablaUtil.ObtenerNumElementos()
            << ", Capacidad=" << tablaUtil.ObtenerCapacidad()
            << ", FactorCarga=" << tablaUtil.FactorCarga() << std::endl;
    tablaUtil.MostrarTabla();

    std::cout << "\nInsertando 3 elementos en tablaUtil..." << std::endl;
    tablaUtil.Insertar("uno", 1);
    tablaUtil.Insertar("dos", 2);
    tablaUtil.Insertar("tres", 3);
    std::cout << "Despues de insertar:" << std::endl;
    std::cout << "  EstaVacia()=" << (tablaUtil.EstaVacia() ? "true" : "false")
            << ", Elementos=" << tablaUtil.ObtenerNumElementos()
            << ", Capacidad=" << tablaUtil.ObtenerCapacidad()
            << ", FactorCarga=" << tablaUtil.FactorCarga() << std::endl;
    tablaUtil.MostrarTabla();

    std::cout << "\nInsertando 1 elemento mas (esperando rehash si FC > 0.75)..." << std::endl;
    tablaUtil.Insertar("cuatro", 4);
    std::cout << "Despues de insertar 'cuatro':" << std::endl;
    std::cout << "  EstaVacia()=" << (tablaUtil.EstaVacia() ? "true" : "false")
            << ", Elementos=" << tablaUtil.ObtenerNumElementos()
            << ", Capacidad=" << tablaUtil.ObtenerCapacidad()
            << ", FactorCarga=" << tablaUtil.FactorCarga() << std::endl;
    tablaUtil.MostrarTabla();

    std::cout << "\nLlamando a tablaUtil.Vaciar()..." << std::endl;
    tablaUtil.Vaciar();
    std::cout << "Despues de Vaciar():" << std::endl;
    std::cout << "  EstaVacia()=" << (tablaUtil.EstaVacia() ? "true" : "false")
            << ", Elementos=" << tablaUtil.ObtenerNumElementos()
            << ", Capacidad=" << tablaUtil.ObtenerCapacidad()
            << ", FactorCarga=" << tablaUtil.FactorCarga() << std::endl;
    tablaUtil.MostrarTabla();

    std::cout << "\nInsertando en tablaUtil despues de Vaciar()..." << std::endl;
    tablaUtil.Insertar("nuevo_uno", 101);
    tablaUtil.Insertar("nuevo_dos", 202);
    std::cout << "Despues de insertar 2 elementos en tablaUtil vaciada:" << std::endl;
    std::cout << "  EstaVacia()=" << (tablaUtil.EstaVacia() ? "true" : "false")
            << ", Elementos=" << tablaUtil.ObtenerNumElementos()
            << ", Capacidad=" << tablaUtil.ObtenerCapacidad()
            << ", FactorCarga=" << tablaUtil.FactorCarga() << std::endl;
    tablaUtil.MostrarTabla();
    std::cout << "--- FIN: Pruebas de Metodos Utilitarios ---" << std::endl;
}
