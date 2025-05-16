/**
 * \file main.cpp
 * Implementación del programa principal para demostrar el uso de la clase TablaHash.
 * Permite interacción vía consola con menú para realizar operaciones sobre la tabla hash.
 * \author Joan Antonio Lazaro Silva
 * \date 06/05/2025
 */

#include "../Headers/TablaHash.hpp" // Incluir el header de la clase

#include <iostream>
#include <string>
#include <chrono>
#include <limits>
#include <random>
#include <vector>
#include <iomanip>
#include <limits> // Para std::numeric_limits
#include <algorithm> // Para std::min


// Usar el namespace para no tener que cualificar TablaHash y MetodoColision
using namespace myhashtable;


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
    // Usar los nombres del enum directamente (ya que usamos 'using namespace')
    std::cout << "1. Encadenamiento\n";
    std::cout << "2. Sondeo Lineal\n";
    std::cout << "3. Sondeo Cuadrático\n";
    std::cout << "4. Doble Hash\n";
    std::cout << "Seleccione un método: ";
    imprimirMarco();
}

/** \brief Limpia el buffer de entrada estándar para evitar errores */
void limpiarBuffer() {
    // Ignora hasta el final de la línea actual en el buffer de entrada
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * \brief Solicita y obtiene un número entero desde consola
 * Incluye validación de entrada básica.
 * \param mensaje Mensaje que se muestra para solicitar el entero
 * \return Entero ingresado por el usuario
 */
int obtenerEntero(const std::string& mensaje) {
    int valor;
    std::cout << mensaje;
    // Intenta leer un entero. Si falla, muestra error y limpia el buffer.
    while (!(std::cin >> valor)) {
        std::cout << "Entrada invalida. Por favor, ingrese un numero entero: ";
        std::cin.clear(); // Limpiar el flag de error de cin
        limpiarBuffer(); // Descartar la entrada incorrecta en el buffer
    }
    limpiarBuffer(); // Limpiar el resto de la línea después de leer el entero exitosamente
    return valor;
}

/**
 * \brief Solicita y obtiene un número flotante desde consola
 * Incluye validación de entrada básica.
 * \param mensaje Mensaje que se muestra para solicitar el flotante
 * \return Valor flotante ingresado por el usuario
 */
float obtenerFlotante(const std::string& mensaje) {
    float valor;
    std::cout << mensaje;
    // Intenta leer un flotante. Si falla, muestra error y limpia el buffer.
    while (!(std::cin >> valor)) {
        std::cout << "Entrada invalida. Por favor, ingrese un numero flotante: ";
        std::cin.clear(); // Limpiar el flag de error de cin
        limpiarBuffer(); // Descartar la entrada incorrecta en el buffer
    }
    limpiarBuffer(); // Limpiar el resto de la línea después de leer el flotante exitosamente
    return valor;
}

/**
 * \brief Solicita y obtiene una cadena desde consola
 * Lee una línea completa de la entrada estándar.
 * \param mensaje Mensaje que se muestra para solicitar la cadena
 * \return Cadena ingresada por el usuario
 */
std::string obtenerCadena(const std::string& mensaje) {
    std::string valor;
    std::cout << mensaje;
    // Lee una línea completa, incluyendo espacios.
    std::getline(std::cin, valor);
    return valor;
}

/**
 * \brief Realiza una prueba de rendimiento insertando y buscando elementos.
 * Utiliza un generador de números aleatorios para simular datos.
 * \tparam K Tipo de clave usado en la tabla hash.
 * \tparam V Tipo de valor usado en la tabla hash.
 * \param tabla Referencia a la tabla hash a evaluar.
 */
template <typename K, typename V>
void pruebaRendimiento(TablaHash<K, V>& tabla) {
    // Asegurar que los tipos K y V sean soportados por la generación de datos de prueba.
    if constexpr (!std::is_same_v<K, int> || !std::is_same_v<V, std::string>) {
        std::cerr << "Error: pruebaRendimiento solo implementada para K=int, V=std::string.\n";
        return;
    }

    std::cout << "\n=== PRUEBA DE RENDIMIENTO (" << tabla.nombreMetodo() << ") ===\n";

    // Usar seed fijo (ej: std::mt19937 gen(0);) para pruebas comparables si es necesario,
    // o seed aleatorio para pruebas más realistas.
    std::random_device rd;
    std::mt19937 gen(rd());
    // Rango de claves más amplio para simular un universo de claves más grande que la tabla.
    std::uniform_int_distribution<> distrib_claves(1, 2000000);

    const int numElementos = obtenerEntero("Numero de elementos a insertar: ");
    if (numElementos <= 0) return;

    tabla.vaciar(); // Empezar con una tabla vacía para cada prueba.

    // Generar claves para inserción. Guardamos una copia para usarlas en la búsqueda.
    std::vector<int> clavesParaInsercion(numElementos);
    for(int i = 0; i < numElementos; ++i) {
        clavesParaInsercion[i] = distrib_claves(gen);
    }

    // --- Prueba de Inserción ---
    std::cout << "Realizando insercion de " << numElementos << " elementos...\n";
    const auto inicioInsercion = std::chrono::high_resolution_clock::now();

    size_t inserciones_exitosas = 0;
    for(int clave : clavesParaInsercion) {
        // Intentar insertar. 'insertar' maneja duplicados y rehash.
        if (tabla.insertar(clave, "Valor" + std::to_string(clave))) {
            inserciones_exitosas++;
        }
    }

    const auto finInsercion = std::chrono::high_resolution_clock::now();
    const auto duracionInsercion = std::chrono::duration_cast<std::chrono::milliseconds>(finInsercion - inicioInsercion); // Medir en milisegundos

    std::cout << "Inserciones exitosas (elementos unicos añadidos): " << inserciones_exitosas << " de " << numElementos << " intentos.\n";
    std::cout << "Tiempo de Insercion: " << duracionInsercion.count() << " ms\n";
    tabla.mostrarEstadisticas(); // Mostrar estadísticas después de la inserción

    // --- Prueba de Búsqueda ---
    // Probar búsqueda con una mezcla de claves que existen y que no.
    // Usaremos algunas de las claves que SÍ se insertaron exitosamente
    // y algunas claves nuevas generadas aleatoriamente.

    std::vector<int> clavesParaBuscar;
    // Número de búsquedas (ej: el doble de elementos insertados, o un número fijo)
    size_t total_busquedas = std::max((size_t)2000, inserciones_exitosas * 2);
    clavesParaBuscar.reserve(total_busquedas);

    // Añadir claves que existen (las primeras 'buscar_existentes_count' de las que intentamos insertar)
    size_t buscar_existentes_count = std::min((size_t)numElementos, total_busquedas / 2); // Ej: la mitad serán claves insertadas
    for(size_t i = 0; i < buscar_existentes_count; ++i) {
        clavesParaBuscar.push_back(clavesParaInsercion[i]);
    }

    // Añadir claves que probablemente NO existen (nuevas claves aleatorias fuera del conjunto insertado)
    size_t buscar_no_existentes_count = total_busquedas - buscar_existentes_count;
    for(size_t i = 0; i < buscar_no_existentes_count; ++i) {
        clavesParaBuscar.push_back(distrib_claves(gen)); // Generar nuevas claves
    }

    // Mezclar las claves para buscar para evitar patrones (opcional pero recomendado)
    std::shuffle(clavesParaBuscar.begin(), clavesParaBuscar.end(), gen);


    std::cout << "\nRealizando busqueda de " << clavesParaBuscar.size() << " claves ("
              << buscar_existentes_count << " supuestamente existentes, "
              << buscar_no_existentes_count << " supuestamente no existentes)...\n";
    const auto inicioBusqueda = std::chrono::high_resolution_clock::now();

    int encontrados = 0;
    for(int clave : clavesParaBuscar) {
        if(tabla.buscar(clave).has_value()) {
            ++encontrados;
        }
    }

    const auto finBusqueda = std::chrono::high_resolution_clock::now();
    const auto duracionBusqueda = std::chrono::duration_cast<std::chrono::milliseconds>(finBusqueda - inicioBusqueda); // Medir en milisegundos

    std::cout << "Claves encontradas: " << encontrados << " de " << clavesParaBuscar.size() << " búsquedas.\n";
    std::cout << "Tiempo de Busqueda: " << duracionBusqueda.count() << " ms\n";
    tabla.mostrarEstadisticas(); // Mostrar estadísticas después de la búsqueda


    // --- Prueba de Eliminación (Opcional) ---
    // Puedes añadir una prueba similar para la eliminación.
    // Eliminar un subconjunto de elementos insertados y algunos que no existen.
    // Por ejemplo:
    /*
    std::cout << "\nRealizando eliminacion...\n";
    std::vector<int> clavesParaEliminar;
    // Añadir algunas claves que existen (ej: la mitad de las insertadas)
    size_t eliminar_existentes_count = inserciones_exitosas / 2;
    for(size_t i = 0; i < eliminar_existentes_count; ++i) {
         clavesParaEliminar.push_back(clavesParaInsercion[i]); // Eliminar las primeras de las insertadas
    }
    // Añadir algunas claves que NO existen
    size_t eliminar_no_existentes_count = 500; // Eliminar 500 claves que no existen
     for(size_t i = 0; i < eliminar_no_existentes_count; ++i) {
         clavesParaEliminar.push_back(distrib_claves(gen)); // Claves aleatorias para eliminar
    }
     std::shuffle(clavesParaEliminar.begin(), clavesParaEliminar.end(), gen);

    const auto inicioEliminacion = std::chrono::high_resolution_clock::now();
    int eliminados_exitosos = 0;
    for(int clave : clavesParaEliminar) {
        if (tabla.eliminar(clave)) {
            eliminados_exitosos++;
        }
    }
    const auto finEliminacion = std::chrono::high_resolution_clock::now();
    const auto duracionEliminacion = std::chrono::duration_cast<std::chrono::milliseconds>(finEliminacion - inicioEliminacion);

    std::cout << "Eliminaciones exitosas: " << eliminados_exitosos << " de " << clavesParaEliminar.size() << " intentos.\n";
    std::cout << "Tiempo de Eliminacion: " << duracionEliminacion.count() << " ms\n";
    tabla.mostrarEstadisticas(); // Mostrar estadísticas finales

    */


    std::cout << "\n=== FIN PRUEBA DE RENDIMIENTO ===\n";

}

/**
 * \brief Función principal que controla el flujo del programa.
 * Presenta un menú interactivo para operar la tabla hash.
 * \return Código de salida (0 para éxito).
 */
int main() {
    // Crear una instancia de TablaHash.
    // Se usa el namespace 'myhashtable'.
    TablaHash<int, std::string> tabla(100, 0.7f, 0.3f, ENCADENAMIENTO);
    // Puedes cambiar el método inicial aquí si quieres:
    // TablaHash<int, std::string> tabla(100, 0.7f, 0.3f, SONDEO_LINEAL);
    // TablaHash<int, std::string> tabla(100, 0.7f, 0.3f, SONDEO_CUADRATICO);
    // TablaHash<int, std::string> tabla(100, 0.7f, 0.3f, DOBLE_HASH);


    bool salir = false;

    while(!salir) {
        imprimirMenu(); // Mostrar el menú principal
        int opcion;
        // Intenta leer la opción del menú
        std::cin >> opcion;

        // Verifica si la lectura fue exitosa. Si no, limpia el error y el buffer, y pide reintentar.
        if (std::cin.fail()) {
            std::cout << "Entrada invalida. Por favor, ingrese un numero correspondiente a una opcion del menu.\n";
            std::cin.clear(); // Limpiar el flag de error
            limpiarBuffer(); // Descartar la entrada incorrecta
            continue; // Ir al siguiente ciclo del bucle while para mostrar el menú de nuevo
        } else {
            limpiarBuffer(); // Limpiar el resto de la línea después de leer el número de opción
        }


        try {
            // Usar un switch para manejar la opción seleccionada
            switch(opcion) {
                case 0:
                    salir = true; // Opción para salir del programa
                    break;
                case 1: {
                    // Insertar elemento
                    int clave = obtenerEntero("Clave (int): ");
                    std::string valor = obtenerCadena("Valor (string): ");
                    if (tabla.insertar(clave, valor)) {
                        std::cout << "Elemento insertado con exito.\n";
                    } else {
                        std::cout << "Error: No se pudo insertar el elemento (la clave ya podria existir).\n";
                    }
                    break;
                }
                case 2: {
                    // Buscar elemento
                    int clave = obtenerEntero("Clave a buscar: ");
                    // Buscar devuelve un std::optional<V>
                    if(auto res = tabla.buscar(clave); res.has_value()) {
                        // Si optional tiene un valor, se encontró el elemento
                        std::cout << "Elemento encontrado. Valor: " << res.value() << "\n";
                    } else {
                        // Si optional no tiene valor, no se encontró la clave
                        std::cout << "Clave no encontrada en la tabla.\n";
                    }
                    break;
                }
                case 3: {
                    // Eliminar elemento
                    int clave = obtenerEntero("Clave a eliminar: ");
                    if (tabla.eliminar(clave)) {
                        std::cout << "Elemento eliminado con exito.\n";
                    } else {
                        std::cout << "Clave no encontrada en la tabla. No se elimino ningun elemento.\n";
                    }
                    break;
                }
                case 4:
                    tabla.mostrar(); // Mostrar el contenido de la tabla
                    break;
                case 5:
                    tabla.mostrarEstadisticas(); // Mostrar las estadísticas de la tabla
                    break;
                case 6:
                    tabla.mostrarDistribucion(); // Mostrar la distribución interna de la tabla
                    break;
                case 7: {
                    // Cambiar método de colisión
                    imprimirMenuMetodos(); // Mostrar el menú de métodos
                    int m;
                    std::cin >> m;
                    // Validar la entrada y limpiar el buffer
                    if (std::cin.good()) {
                        limpiarBuffer();
                        // El enum MetodoColision es 0-indexed, el menú es 1-indexed
                        if(m >=1 && m <=4) {
                            tabla.cambiarMetodo(static_cast<MetodoColision>(m - 1)); // Convertir opción a enum
                            std::cout << "Metodo de colision cambiado a " << tabla.nombreMetodo() << ".\n";
                        } else {
                            std::cout << "Opcion de metodo invalida.\n";
                        }
                    } else {
                        std::cout << "Entrada invalida para seleccion de metodo.\n";
                        std::cin.clear(); // Limpiar el flag de error
                        limpiarBuffer(); // Descartar la entrada incorrecta
                    }
                    break;
                }
                case 8: {
                    // Configurar factor de carga
                    const float max = obtenerFlotante("Factor carga maximo (ej: 0.7): ");
                    const float min = obtenerFlotante("Factor carga minimo (ej: 0.3): ");
                    try {
                        tabla.configurarFactorCarga(max, min); // Configurar, puede lanzar std::invalid_argument
                        std::cout << "Factores de carga configurados.\n";
                    } catch(const std::invalid_argument& e) {
                        // Capturar excepción si los factores son inválidos
                        std::cerr << "Error de configuracion del factor de carga: " << e.what() << "\n";
                    }
                    break;
                }
                case 9: {
                    // Cargar datos de prueba
                    const int cantidad = obtenerEntero("Cantidad de datos de prueba a cargar: ");
                    if (cantidad > 0) {
                        tabla.cargarDatosPrueba(static_cast<size_t>(cantidad)); // Cargar, asume int, string
                        std::cout << "Intento de cargar " << cantidad << " datos de prueba.\n";
                    } else {
                        std::cout << "Cantidad invalida.\n";
                    }
                    break;
                }
                case 10:
                    tabla.vaciar(); // Vaciar la tabla
                    std::cout << "Tabla vaciada completamente.\n";
                    break;
                case 11: {
                    // Guardar tabla en archivo
                    std::string archivo = obtenerCadena("Nombre del archivo para guardar: ");
                    if (!archivo.empty()) {
                        if (tabla.guardarEnArchivo(archivo)) // Guardar, asume serializable a string
                            std::cout << "Tabla guardada en '" << archivo << "'.\n";
                        else
                            std::cerr << "Error al guardar tabla en '" << archivo << "'.\n";
                    } else {
                        std::cout << "Nombre de archivo no puede estar vacio.\n";
                    }
                    break;
                }
                case 12: {
                    // Cargar tabla desde archivo
                    std::string archivo = obtenerCadena("Nombre del archivo para cargar: ");
                    if (!archivo.empty()) {
                        if (tabla.cargarDesdeArchivo(archivo)) // Cargar, asume serializable desde string
                            std::cout << "Tabla cargada desde '" << archivo << "'.\n";
                        else
                            std::cerr << "Error al cargar tabla desde '" << archivo << "'.\n";
                    } else {
                        std::cout << "Nombre de archivo no puede estar vacio.\n";
                    }
                    break;
                }
                case 13: {
                    // Iterar elementos usando range-based for loop
                    std::cout << "\n=== ELEMENTOS DE LA TABLA (usando iterador) ===\n";
                    int contador = 0;
                    // El range-based for loop usa begin() y end()
                    for (const auto& [clave, valor] : tabla) {
                        // Usar any_to_string si V puede no ser imprimible directamente
                        std::cout << contador++ << ": (" << TablaHash<int, std::string>::any_to_string_public(clave) << ", " << TablaHash<int, std::string>::any_to_string_public(valor) << ")\n";
                        // Opcional: Limitar la salida si hay muchos elementos para no saturar la consola
                        if (contador >= 50 && tabla.cantidadElementos() > 50) {
                            std::cout << "... (Mostrando solo los primeros 50 de " << tabla.cantidadElementos() << " elementos validos)\n";
                            break; // Salir del bucle for
                        }
                    }
                    if (contador == 0) {
                        std::cout << "La tabla esta vacia.\n";
                    } else if (contador < tabla.cantidadElementos()) {
                        std::cout << "Se mostraron " << contador << " elementos de un total de " << tabla.cantidadElementos() << " elementos validos.\n";
                    } else {
                        std::cout << "Mostrados todos los " << tabla.cantidadElementos() << " elementos validos.\n";
                    }
                    std::cout << "--- Fin de la Iteracion ---\n";
                    break;
                }
                case 14:
                    tabla.toggleDebugMode(); // Activar/desactivar modo debug
                    break;
                case 15:
                    // Ejecutar prueba de rendimiento
                    pruebaRendimiento(tabla); // Asume int, string
                    break;
                default:
                    // Opción no reconocida del menú
                    std::cout << "Opcion no valida. Por favor, intente de nuevo.\n";
                    break;
            }
        } catch(const std::invalid_argument& e) {
            // Captura excepciones de argumentos inválidos (ej. configurarFactorCarga)
            std::cerr << "Error de argumento: " << e.what() << "\n";
        } catch(const std::runtime_error& e) {
            // Captura excepciones de errores en tiempo de ejecución (ej. string_to_any)
            std::cerr << "Error de ejecucion: " << e.what() << "\n";
        } catch(const std::exception& e) {
            // Captura otras excepciones estándar
            std::cerr << "Error inesperado: " << e.what() << "\n";
        } catch(...) {
            // Captura cualquier otra excepción no estándar
            std::cerr << "Error desconocido ocurrido.\n";
        }

        // Esperar a que el usuario presione Enter antes de mostrar el menú de nuevo,
        // a menos que haya salido del programa.
        if (!salir) {
            std::cout << "\nPresione Enter para continuar...\n";
            std::cin.get(); // Espera por una nueva línea (después de limpiarBuffer en la lectura de opción)
        }
    }

    std::cout << "Saliendo del programa. Gracias por usar la TablaHash.\n";
    return 0; // Indicar salida exitosa
}