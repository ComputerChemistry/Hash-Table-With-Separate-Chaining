/**  
 * \file main.cpp
 * Implementación del programa principal para demostrar el uso de la clase TablaHash.
 * Permite interacción vía consola con menú para realizar operaciones sobre la tabla hash.
 * \author Joan Antonio Lazaro Silva
 * \date 06/05/2025
 */

#include "TablaHash.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <limits>
#include <random>
#include <vector>
#include <iomanip>

/** \brief Imprime un marco decorativo en consola */
void imprimirMarco() {
    std::cout << "+-----------------------------------------------------------+\n";
}

/** \brief Imprime el menú principal con las opciones disponibles */
void imprimirMenu() {
    imprimirMarco();
    std::cout << "           DEMOSTRACIÓN DE TABLA HASH\n";
    imprimirMarco();
    std::cout << "1. Insertar elemento\n";
    std::cout << "2. Buscar elemento\n";
    std::cout << "3. Eliminar elemento\n";
    std::cout << "4. Mostrar tabla\n";
    std::cout << "5. Estadísticas\n";
    std::cout << "6. Mostrar distribución\n";
    std::cout << "7. Cambiar método de resolución de colisiones\n";
    std::cout << "8. Configurar factor de carga\n";
    std::cout << "9. Cargar datos de prueba\n";
    std::cout << "10. Vaciar tabla\n";
    std::cout << "11. Guardar tabla en archivo\n";
    std::cout << "12. Cargar tabla desde archivo\n";
    std::cout << "13. Iterar elementos\n";
    std::cout << "14. Modo debug ON/OFF\n";
    std::cout << "15. Prueba de rendimiento\n";
    std::cout << "0. Salir\n";
    std::cout << "Ingrese su opción: ";
    imprimirMarco();
}

/** \brief Imprime el menú para seleccionar el método de resolución */
void imprimirMenuMetodos() {
    imprimirMarco();
    std::cout << "       MÉTODOS DE RESOLUCIÓN DE COLISIONES\n";
    imprimirMarco();
    std::cout << "1. Encadenamiento\n";
    std::cout << "2. Sondeo Lineal\n";
    std::cout << "3. Sondeo Cuadrático\n";
    std::cout << "4. Doble Hash\n";
    std::cout << "Seleccione un método: ";
    imprimirMarco();
}

/** \brief Limpia el buffer de entrada estándar para evitar errores */
void limpiarBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/** 
 * \brief Solicita y obtiene un número entero desde consola
 * \param mensaje Mensaje que se muestra para solicitar el entero
 * \return Entero ingresado por el usuario
 */
int obtenerEntero(const std::string& mensaje) {
    int valor;
    std::cout << mensaje;
    while (!(std::cin >> valor)) {
        std::cout << "Entrada inválida. Intente de nuevo: ";
        limpiarBuffer();
    }
    limpiarBuffer();
    return valor;
}

/** 
 * \brief Solicita y obtiene un número flotante desde consola
 * \param mensaje Mensaje que se muestra para solicitar el flotante
 * \return Valor flotante ingresado por el usuario
 */
float obtenerFlotante(const std::string& mensaje) {
    float valor;
    std::cout << mensaje;
    while (!(std::cin >> valor)) {
        std::cout << "Entrada inválida. Intente de nuevo: ";
        limpiarBuffer();
    }
    limpiarBuffer();
    return valor;
}

/** 
 * \brief Solicita y obtiene una cadena desde consola
 * \param mensaje Mensaje que se muestra para solicitar la cadena
 * \return Cadena ingresada por el usuario
 */
std::string obtenerCadena(const std::string& mensaje) {
    std::string valor;
    std::cout << mensaje;
    std::getline(std::cin, valor);
    return valor;
}

/** 
 * \brief Realiza una prueba de rendimiento insertando y buscando elementos
 * \tparam K Tipo de clave usado en la tabla hash
 * \tparam V Tipo de valor usado en la tabla hash
 * \param tabla Referencia a la tabla hash a evaluar
 */
template <typename K, typename V>
void pruebaRendimiento(TablaHash<K, V>& tabla) {
    std::cout << "\n=== PRUEBA DE RENDIMIENTO ===\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 1000000);

    const int numElementos = obtenerEntero("Número de elementos a insertar: ");

    tabla.vaciar();

    const auto inicioInsercion = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < numElementos; ++i) {
        int clave = distrib(gen);
        tabla.insertar(clave, "Valor" + std::to_string(clave));
    }

    const auto finInsercion = std::chrono::high_resolution_clock::now();
    const auto duracionInsercion = std::chrono::duration_cast<std::chrono::microseconds>(finInsercion - inicioInsercion);

    std::vector<int> clavesParaBuscar(1000);
    for(auto& c : clavesParaBuscar) c = distrib(gen);

    const auto inicioBusqueda = std::chrono::high_resolution_clock::now();

    int encontrados = 0;
    for(int clave : clavesParaBuscar) {
        if(tabla.buscar(clave).has_value()) ++encontrados;
    }

    const auto finBusqueda = std::chrono::high_resolution_clock::now();
    const auto duracionBusqueda = std::chrono::duration_cast<std::chrono::microseconds>(finBusqueda - inicioBusqueda);

    std::cout << "Método: " << tabla.nombreMetodo() << "\n";
    std::cout << "Inserción: " << static_cast<double>(duracionInsercion.count())/1000.0 << " ms\n";
    std::cout << "Búsqueda 1000 claves: " << static_cast<double>(duracionBusqueda.count())/1000.0 << " ms\n";
    std::cout << "Encontrados: " << encontrados << " de 1000\n";

    tabla.mostrarEstadisticas();
}

/** 
 * \brief Función principal que controla el flujo del programa
 * \return Código de salida (0 para éxito)
 */
int main() {
    TablaHash<int, std::string> tabla(50, 0.7f, 0.3f, DOBLE_HASH);

    bool salir = false;

    while(!salir) {
        imprimirMenu();
        int opcion;
        std::cin >> opcion;
        limpiarBuffer();

        try {
            switch(opcion) {
                case 0: salir = true; break;
                case 1: {
                    int clave = obtenerEntero("Clave (int): ");
                    if (std::string valor = obtenerCadena("Valor (string): "); tabla.insertar(clave, valor))
                        std::cout << "Elemento insertado.\n";
                    else
                        std::cout << "Error al insertar (clave posiblemente duplicada).\n";
                    break;
                }
                case 2: {
                    int clave = obtenerEntero("Clave a buscar: ");
                    if(auto res = tabla.buscar(clave); res.has_value())
                        std::cout << "Valor: " << res.value() << "\n";
                    else
                        std::cout << "Clave no encontrada.\n";
                    break;
                }
                case 3: {
                    if (int clave = obtenerEntero("Clave a eliminar: "); tabla.eliminar(clave))
                        std::cout << "Elemento eliminado.\n";
                    else
                        std::cout << "Clave no encontrada.\n";
                    break;
                }
                case 4: tabla.mostrar(); break;
                case 5: tabla.mostrarEstadisticas(); break;
                case 6: tabla.mostrarDistribucion(); break;
                case 7: {
                    imprimirMenuMetodos();
                    int m; std::cin >> m; limpiarBuffer();
                    if(m >=1 && m <=4) {
                        tabla.cambiarMetodo(static_cast<MetodoColision>(m - 1));
                        std::cout << "Método cambiado.\n";
                    } else {
                        std::cout << "Opción inválida.\n";
                    }
                    break;
                }
                case 8: {
                    const float max = obtenerFlotante("Factor carga max (0.4-0.95): ");
                    const float min = obtenerFlotante("Factor carga min (0.1-0.5): ");
                    try {
                        tabla.configurarFactorCarga(max, min);
                        std::cout << "Factor de carga configurado.\n";
                    } catch(const std::invalid_argument& e) {
                        std::cout << "Error: " << e.what() << "\n";
                    }
                    break;
                }
                case 9: {
                    const int cantidad = obtenerEntero("Cantidad de datos de prueba: ");
                    tabla.cargarDatosPrueba(static_cast<size_t>(cantidad));
                    std::cout << "Datos cargados.\n";
                    break;
                }
                case 10:
                    tabla.vaciar();
                    std::cout << "Tabla vaciada.\n";
                    break;
                case 11: {
                    if (std::string archivo = obtenerCadena("Nombre archivo para guardar: "); tabla.guardarEnArchivo(archivo))
                        std::cout << "Tabla guardada.\n";
                    else
                        std::cout << "Error al guardar tabla.\n";
                    break;
                }
                case 12: {
                    if (std::string archivo = obtenerCadena("Nombre archivo para cargar: "); tabla.cargarDesdeArchivo(archivo))
                        std::cout << "Tabla cargada.\n";
                    else
                        std::cout << "Error al cargar tabla.\n";
                    break;
                }
                case 13: {
                    std::cout << "\n=== ELEMENTOS DE LA TABLA ===\n";
                    int contador = 0;
                    for (const auto& [clave, valor] : tabla) {
                        std::cout << contador++ << ": (" << clave << ", " << valor << ")\n";
                        if (contador >= 20) {
                            std::cout << "... (hay más elementos, solo se muestran los primeros 20)\n";
                            break;
                        }
                    }
                    if (contador == 0) {
                        std::cout << "La tabla está vacía.\n";
                    }
                    break;
                }
                case 14:
                    tabla.toggleDebugMode();
                    break;
                case 15:
                    pruebaRendimiento(tabla);
                    break;
                default:
                    std::cout << "Opción no válida.\n";
                    break;
            }
        } catch(const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }

    std::cout << "Gracias por usar la TablaHash.\n";
    return 0;
}
