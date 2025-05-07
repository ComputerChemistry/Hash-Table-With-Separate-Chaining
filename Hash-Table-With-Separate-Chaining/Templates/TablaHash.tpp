/**  
 * \file TablaHash.tpp
 * Este archivo contiene las implementaciones de la clase template `TablaHash`.
 * Define el comportamiento de métodos para manejar tablas hash con diferentes técnicas de colisión.
 * \author Joan Antonio Lazaro Silva
 * \date 06/05/2025
 */

#ifndef TABLA_HASH_TPP
#define TABLA_HASH_TPP

#include <sstream>
#include <cmath>
#include <random>
#include <iomanip>
#include <fstream>
#include <iostream>

template <typename K, typename V>
/** \brief Constructor por defecto de HashEntry, marca la entrada como no ocupada ni eliminada */
TablaHash<K, V>::HashEntry::HashEntry() : ocupado(false), eliminado(false) {}

template <typename K, typename V>
/** \brief Constructor de HashEntry con clave y valor, marca la entrada como ocupada */
TablaHash<K, V>::HashEntry::HashEntry(const K& k, const V& v)
    : clave(k), valor(v), ocupado(true), eliminado(false) {}

template <typename K, typename V>
/** \brief Inicializa las estadísticas a cero */
TablaHash<K, V>::Estadisticas::Estadisticas()
    : inserciones(0), busquedas(0), eliminaciones(0), colisiones(0), rehashes(0) {}

template <typename K, typename V>
/** \brief Reinicia todas las estadísticas a cero */
void TablaHash<K, V>::Estadisticas::reset() {
    inserciones = 0;
    busquedas = 0;
    eliminaciones = 0;
    colisiones = 0;
    rehashes = 0;
}

template <typename K, typename V>
/** 
 * \brief Constructor principal de TablaHash
 * \param capacidadInicial Capacidad inicial mínima (se asegura que sea al menos CAPACIDAD_MINIMA)
 * \param factorMax Factor de carga máximo, entre 0.4 y 0.95
 * \param factorMin Factor de carga mínimo, entre 0.1 y 0.5
 * \param metodoColision Método de resolución de colisiones a usar
 * \throw invalid_argument Si los factores de carga no cumplen los rangos o si min > max
 */
TablaHash<K, V>::TablaHash(const size_t capacidadInicial, const float factorMax, const float factorMin, const MetodoColision metodoColision)
    : capacidad(std::max(capacidadInicial, CAPACIDAD_MINIMA)),
      elementos(0),
      factorCargaMax(factorMax),
      factorCargaMin(factorMin),
      metodo(metodoColision),
      debugMode(false) {
    if (factorMax < FACTOR_MAX_MINIMO || factorMax > FACTOR_MAX_MAXIMO)
        throw std::invalid_argument("El factor de carga máximo debe estar entre 0.4 y 0.95.");
    if (factorMin < FACTOR_MIN_MINIMO || factorMin > FACTOR_MIN_MAXIMO)
        throw std::invalid_argument("El factor de carga mínimo debe estar entre 0.1 y 0.5.");
    if (factorMin > factorMax)
        throw std::invalid_argument("El factor de carga mínimo no puede ser mayor que el máximo.");

    tabla.resize(capacidad);
    listas.resize(capacidad);
    stats.reset();
}

template <typename K, typename V>
/** \brief Función hash principal que utiliza std::hash y módulo con la capacidad */
size_t TablaHash<K, V>::funcionHash(const K& clave) const {
    std::hash<K> hashFunc;
    return hashFunc(clave) % capacidad;
}

template <typename K, typename V>
/** \brief Función hash secundaria usada en doble hashing */
size_t TablaHash<K, V>::funcionHashSecundaria(const K& clave) const {
    std::hash<K> hashFunc;
    const size_t h2 = 1 + (hashFunc(clave) % (capacidad - 1));
    return h2;
}

template <typename K, typename V>
/** \brief Calcula el factor de carga actual */
float TablaHash<K, V>::factorCarga() const {
    return static_cast<float>(elementos) / capacidad;
}

template <typename K, typename V>
/**
 * \brief Realiza un rehash a una nueva capacidad
 * \param nuevaCapacidad nueva capacidad para la tabla hash
 * \details Reposiciona todos los elementos existentes según la nueva capacidad y método de colisión.
 */
void TablaHash<K, V>::rehash(const size_t nuevaCapacidad) {
    if (debugMode) {
        std::cout << "Rehashing desde capacidad " << capacidad << " a " << nuevaCapacidad << std::endl;
    }
    capacidad = nuevaCapacidad;
    ++stats.rehashes;
    elementos = 0;

    auto viejaTabla = std::move(tabla);
    auto viejasListas = std::move(listas);

    tabla.clear();
    listas.clear();
    tabla.resize(capacidad);
    listas.resize(capacidad);

    if (metodo == ENCADENAMIENTO) {
        for (auto& lista : viejasListas) {
            for (auto& par : lista) {
                insertar(par.first, par.second);
            }
        }
    } else {
        for (auto& entry : viejaTabla) {
            if (entry.ocupado && !entry.eliminado) {
                insertar(entry.clave, entry.valor);
            }
        }
    }
}

template <typename K, typename V>
/** \brief Template auxiliar que convierte un valor cualquiera a string usando stringstream */
template <typename T>
std::string TablaHash<K, V>::any_to_string(const T& valor) {
    std::ostringstream oss;
    oss << valor;
    return oss.str();
}

template <typename K, typename V>
/** \brief Template auxiliar que convierte un string a un tipo T usando stringstream */
template <typename T>
T TablaHash<K, V>::string_to_any(const std::string& str) const {
    std::istringstream iss(str);
    T valor;
    iss >> valor;
    return valor;
}

template <typename K, typename V>
/**
 * \brief Inserta un elemento en la tabla hash
 * \param clave Clave del elemento
 * \param valor Valor asociado
 * \return true si la inserción fue exitosa, false si la clave ya existía
 * \details Realiza rehash si el factor de carga excede el máximo configurado.
 */
bool TablaHash<K, V>::insertar(const K& clave, const V& valor) {
    if (factorCarga() > factorCargaMax)
        rehash(capacidad * 2);

    ++stats.inserciones;

    if (metodo == ENCADENAMIENTO) {
        auto& lista = listas[funcionHash(clave)];
        for (auto& par : lista) {
            if (par.first == clave) {
                if (debugMode)
                    std::cout << "Clave ya existente, no insertada." << std::endl;
                return false;
            }
        }
        lista.emplace_back(clave, valor);
        ++elementos;
        return true;
    }

    const size_t idx = funcionHash(clave);
    size_t i = 0;
    size_t posicion = idx;
    const size_t offset = funcionHashSecundaria(clave);

    while (true) {
        if (HashEntry& entry = tabla[posicion]; !entry.ocupado || entry.eliminado) {
            tabla[posicion] = HashEntry(clave, valor);
            ++elementos;
            return true;
        } else if (entry.clave == clave) {
            if (debugMode)
                std::cout << "Clave ya existente, no insertada." << std::endl;
            return false;
        } else {
            ++stats.colisiones;
        }
        ++i;
        switch (metodo) {
            case SONDEO_LINEAL:
                posicion = (idx + i) % capacidad;
                break;
            case SONDEO_CUADRATICO:
                posicion = (idx + i * i) % capacidad;
                break;
            case DOBLE_HASH:
                posicion = (idx + i * offset) % capacidad;
                break;
            default:
                throw std::logic_error("Método de colisión desconocido");
        }
        if (i >= capacidad) return false;
    }
}

template <typename K, typename V>
/**
 * \brief Busca un elemento en la tabla por clave
 * \param clave Clave a buscar
 * \return valor asociado envuelto en std::optional, o nullopt si no existe
 */
std::optional<V> TablaHash<K, V>::buscar(const K& clave) const {
    ++stats.busquedas;

    if (metodo == ENCADENAMIENTO) {
        for (const auto& lista = listas[funcionHash(clave)]; const auto& par : lista) {
            if (par.first == clave)
                return par.second;
        }
        return std::nullopt;
    }

    const size_t idx = funcionHash(clave);
    size_t i = 0;
    size_t posicion = idx;
    const size_t offset = funcionHashSecundaria(clave);

    while (true) {
        const HashEntry& entry = tabla[posicion];
        if (!entry.ocupado) return std::nullopt;
        if (!entry.eliminado && entry.clave == clave) return entry.valor;
        ++i;
        switch (metodo) {
            case SONDEO_LINEAL:
                posicion = (idx + i) % capacidad;
                break;
            case SONDEO_CUADRATICO:
                posicion = (idx + i * i) % capacidad;
                break;
            case DOBLE_HASH:
                posicion = (idx + i * offset) % capacidad;
                break;
            default:
                throw std::logic_error("Método de colisión desconocido");
        }
        if (i >= capacidad) return std::nullopt;
    }
}

template <typename K, typename V>
/**
 * \brief Elimina un elemento de la tabla por su clave
 * \param clave Clave del elemento a borrar
 * \return true si la clave fue encontrada y eliminada, false si no
 */
bool TablaHash<K, V>::eliminar(const K& clave) {
    if (metodo == ENCADENAMIENTO) {
        auto& lista = listas[funcionHash(clave)];
        for (auto it = lista.begin(); it != lista.end(); ++it) {
            if (it->first == clave) {
                lista.erase(it);
                --elementos;
                ++stats.eliminaciones;
                return true;
            }
        }
        return false;
    }

    const size_t idx = funcionHash(clave);
    size_t i = 0;
    size_t posicion = idx;
    const size_t offset = funcionHashSecundaria(clave);

    while (true) {
        HashEntry& entry = tabla[posicion];
        if (!entry.ocupado) return false;
        if (!entry.eliminado && entry.clave == clave) {
            entry.eliminado = true;
            --elementos;
            ++stats.eliminaciones;
            return true;
        }
        ++i;
        switch (metodo) {
            case SONDEO_LINEAL:
                posicion = (idx + i) % capacidad;
                break;
            case SONDEO_CUADRATICO:
                posicion = (idx + i * i) % capacidad;
                break;
            case DOBLE_HASH:
                posicion = (idx + i * offset) % capacidad;
                break;
            default:
                throw std::logic_error("Método de colisión desconocido");
        }
        if (i >= capacidad) return false;
    }
}

template <typename K, typename V>
/**
 * \brief Muestra todos los elementos de la tabla en formato tabular
 */
void TablaHash<K, V>::mostrar() const {
    constexpr int anchoClave = 15;
    constexpr int anchoValor = 30;
    const std::string borde = "+" + std::string(anchoClave + 2, '-') + "+" + std::string(anchoValor + 2, '-') + "+";

    std::cout << borde << "\n";
    std::cout << "| Clave           | Valor                        |\n";
    std::cout << borde << "\n";

    if (metodo == ENCADENAMIENTO) {
        for (size_t i = 0; i < capacidad; ++i) {
            if (!listas[i].empty()) {
                for (const auto& par : listas[i]) {
                    std::cout << "| " << std::setw(anchoClave) << par.first << " | " << std::setw(anchoValor) << par.second << " |\n"
                              << borde << "\n";
                }
            }
        }
    } else {
        for (const auto& entry : tabla) {
            if (entry.ocupado && !entry.eliminado) {
                std::cout << "| " << std::setw(anchoClave) << entry.clave << " | " << std::setw(anchoValor) << entry.valor << " |\n"
                          << borde << "\n";
            }
        }
    }
}

template <typename K, typename V>
/**
 * \brief Muestra estadísticas básicas de la tabla hash
 */
void TablaHash<K, V>::mostrarEstadisticas() const {
    std::cout << "\n=== Estadísticas ===\n";
    std::cout << "Inserciones: " << stats.inserciones << "\n";
    std::cout << "Búsquedas: " << stats.busquedas << "\n";
    std::cout << "Eliminaciones: " << stats.eliminaciones << "\n";
    std::cout << "Colisiones: " << stats.colisiones << "\n";
    std::cout << "Rehashes: " << stats.rehashes << "\n";
    std::cout << "Elementos actuales: " << elementos << "\n";
    std::cout << "Capacidad total: " << capacidad << "\n";
    std::cout << "Factor de carga: " << factorCarga() << "\n";
    std::cout << "Método actual: " << nombreMetodo() << "\n\n";
}

template <typename K, typename V>
/**
 * \brief Muestra la distribución de elementos según el método de colisión
 */
void TablaHash<K, V>::mostrarDistribucion() const {
    std::cout << "\n=== Distribución ===\n";
    if (metodo == ENCADENAMIENTO) {
        for (size_t i = 0; i < capacidad; ++i) {
            std::cout << "Bucket " << i << ": " << listas[i].size() << " elementos\n";
        }
    } else {
        size_t ocupados = 0;
        for (const auto& entry : tabla) {
            if (entry.ocupado && !entry.eliminado)
                ++ocupados;
        }
        std::cout << "Elementos ocupados: " << ocupados << " de " << capacidad << "\n";
    }
}

template <typename K, typename V>
/**
 * \brief Cambia el método de resolución de colisiones y rehace la tabla
 * \param nuevoMetodo Nuevo método a aplicar
 */
void TablaHash<K, V>::cambiarMetodo(const MetodoColision nuevoMetodo) {
    if (nuevoMetodo != metodo) {
        metodo = nuevoMetodo;
        rehash(capacidad);
    }
}

template <typename K, typename V>
/**
 * \brief Configura los factores máximo y mínimo de carga de la tabla
 * \param nuevoFactorMax Nuevo factor máximo (entre 0.4 y 0.95)
 * \param nuevoFactorMin Nuevo factor mínimo (entre 0.1 y 0.5)
 * \throw invalid_argument Si los factores no cumplen las restricciones
 */
void TablaHash<K, V>::configurarFactorCarga(const float nuevoFactorMax, const float nuevoFactorMin) {
    if (nuevoFactorMax < FACTOR_MAX_MINIMO || nuevoFactorMax > FACTOR_MAX_MAXIMO)
        throw std::invalid_argument("Factor carga máximo debe estar entre 0.4 y 0.95");
    if (nuevoFactorMin < FACTOR_MIN_MINIMO || nuevoFactorMin > FACTOR_MIN_MAXIMO)
        throw std::invalid_argument("Factor carga mínimo debe estar entre 0.1 y 0.5");
    if (nuevoFactorMin > nuevoFactorMax)
        throw std::invalid_argument("Factor carga mínimo no puede ser mayor que el máximo");

    factorCargaMax = nuevoFactorMax;
    factorCargaMin = nuevoFactorMin;
}

template <typename K, typename V>
/**
 * \brief Carga elementos de prueba aleatorios en la tabla
 * \param cantidad Número de elementos a insertar
 */
void TablaHash<K, V>::cargarDatosPrueba(const size_t cantidad) {
    vaciar();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 1000000);

    for (size_t i = 0; i < cantidad; ++i) {
        insertar(distrib(gen), "Valor" + std::to_string(i));
    }
}

template <typename K, typename V>
/** \brief Vacía completamente la tabla y resetea estadísticas */
void TablaHash<K, V>::vaciar() {
    for (size_t i = 0; i < capacidad; ++i) {
        listas[i].clear();
        tabla[i] = HashEntry();
    }
    elementos = 0;
    stats.reset();
}

template <typename K, typename V>
/**
 * \brief Guarda la tabla en un archivo de texto
 * \param nombreArchivo Nombre del archivo donde se guardará la tabla
 * \return true si se guardó correctamente, false en caso contrario
 */
bool TablaHash<K, V>::guardarEnArchivo(const std::string& nombreArchivo) {
    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        return false;
    }

    archivo << capacidad << '\n' << elementos << '\n' << static_cast<int>(metodo) << '\n';

    if (metodo == ENCADENAMIENTO) {
        for (const auto& lista : listas) {
            archivo << lista.size() << '\n';
            for (const auto& par : lista) {
                archivo << any_to_string(par.first) << ' ' << any_to_string(par.second) << '\n';
            }
        }
    } else {
        for (const auto& entry : tabla) {
            archivo << entry.ocupado << ' ' << entry.eliminado << ' ';
            if (entry.ocupado && !entry.eliminado) {
                archivo << any_to_string(entry.clave) << ' ' << any_to_string(entry.valor);
            }
            archivo << '\n';
        }
    }

    archivo.close();
    return true;
}

template <typename K, typename V>
/**
 * \brief Carga una tabla desde un archivo de texto
 * \param nombreArchivo Nombre del archivo a cargar
 * \return true si se cargó correctamente, false en caso contrario
 */
bool TablaHash<K, V>::cargarDesdeArchivo(const std::string& nombreArchivo) {
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        return false;
    }

    size_t cap, elems;
    int metodoInt;

    archivo >> cap >> elems >> metodoInt;
    if (cap < CAPACIDAD_MINIMA || metodoInt < 0 || metodoInt > 3) {
        return false;
    }

    capacidad = cap;
    elementos = elems;
    metodo = static_cast<MetodoColision>(metodoInt);

    tabla.clear();
    listas.clear();
    tabla.resize(capacidad);
    listas.resize(capacidad);

    if (metodo == ENCADENAMIENTO) {
        for (size_t i = 0; i < capacidad; ++i) {
            size_t tam;
            archivo >> tam;
            for (size_t j = 0; j < tam; ++j) {
                std::string claveStr, valorStr;
                archivo >> claveStr >> valorStr;
                K clave = string_to_any<K>(claveStr);
                V valor = string_to_any<V>(valorStr);
                listas[i].emplace_back(clave, valor);
            }
        }
    } else {
        for (size_t i = 0; i < capacidad; ++i) {
            bool ocupado, eliminado;
            archivo >> ocupado >> eliminado;
            tabla[i].ocupado = ocupado;
            tabla[i].eliminado = eliminado;
            if (ocupado && !eliminado) {
                std::string claveStr, valorStr;
                archivo >> claveStr >> valorStr;
                tabla[i].clave = string_to_any<K>(claveStr);
                tabla[i].valor = string_to_any<V>(valorStr);
            }
        }
    }

    archivo.close();
    return true;
}

template <typename K, typename V>
/** \brief Activa o desactiva el modo debug */
void TablaHash<K, V>::toggleDebugMode() {
    debugMode = !debugMode;
    std::cout << "Modo debug " << (debugMode ? "activado." : "desactivado.") << "\n";
}

template <typename K, typename V>
/** \brief Devuelve el nombre del método actual de resolución de colisiones */
std::string TablaHash<K, V>::nombreMetodo() const {
    switch (metodo) {
        case ENCADENAMIENTO: return "Encadenamiento";
        case SONDEO_LINEAL: return "Sondeo Lineal";
        case SONDEO_CUADRATICO: return "Sondeo Cuadrático";
        case DOBLE_HASH: return "Doble Hash";
        default: return "Desconocido";
    }
}

// Implementación de la clase iteradora Iterator

template <typename K, typename V>
/** \brief Busca el siguiente elemento válido para el iterador */
void TablaHash<K, V>::Iterator::encontrarSiguienteElemento() {
    while (indice < tabla->capacidad) {
        if (tabla->metodo == ENCADENAMIENTO) {
            if (listIt != tabla->listas[indice].end()) {
                return;
            }
            ++indice;
            if (indice < tabla->capacidad)
                listIt = tabla->listas[indice].begin();
        } else {
            if (tabla->tabla[indice].ocupado && !tabla->tabla[indice].eliminado) {
                return;
            }
            ++indice;
        }
    }
}

template <typename K, typename V>
/** \brief Constructor del iterador */
TablaHash<K, V>::Iterator::Iterator(TablaHash<K, V>* t, const size_t i)
    : tabla(t), indice(i) {
    if (tabla->metodo == ENCADENAMIENTO) {
        if (indice < tabla->capacidad) {
            listIt = tabla->listas[indice].begin();
            if (listIt == tabla->listas[indice].end())
                encontrarSiguienteElemento();
        }
    } else {
        encontrarSiguienteElemento();
    }
}

template <typename K, typename V>
/** \brief Operador de desreferencia para obtener el elemento actual */
std::pair<K, V> TablaHash<K, V>::Iterator::operator*() const {
    if (tabla->metodo == ENCADENAMIENTO) {
        return *listIt;
    } else {
        const auto& entry = tabla->tabla[indice];
        return {entry.clave, entry.valor};
    }
}

template <typename K, typename V>
/** \brief Operador de incremento para avanzar el iterador */
typename TablaHash<K, V>::Iterator& TablaHash<K, V>::Iterator::operator++() {
    if (tabla->metodo == ENCADENAMIENTO) {
        ++listIt;
        if (listIt == tabla->listas[indice].end()) {
            ++indice;
            if (indice < tabla->capacidad) {
                listIt = tabla->listas[indice].begin();
                encontrarSiguienteElemento();
            }
        }
    } else {
        ++indice;
        encontrarSiguienteElemento();
    }
    return *this;
}

template <typename K, typename V>
/** \brief Operador de igualdad */
bool TablaHash<K, V>::Iterator::operator==(const Iterator& otro) const {
    return tabla == otro.tabla && indice == otro.indice &&
        (tabla->metodo != ENCADENAMIENTO || listIt == otro.listIt);
}

template <typename K, typename V>
/** \brief Operador de desigualdad */
bool TablaHash<K, V>::Iterator::operator!=(const Iterator& otro) const {
    return !(*this == otro);
}

template <typename K, typename V>
/** \brief Devuelve el iterador al inicio */
typename TablaHash<K, V>::Iterator TablaHash<K, V>::begin() {
    if (metodo == ENCADENAMIENTO) {
        size_t start = 0;
        while (start < capacidad && listas[start].empty()) ++start;
        return Iterator(this, start);
    } else {
        size_t start = 0;
        while (start < capacidad && !(tabla[start].ocupado && !tabla[start].eliminado)) ++start;
        return Iterator(this, start);
    }
}

template <typename K, typename V>
/** \brief Devuelve el iterador al final */
typename TablaHash<K, V>::Iterator TablaHash<K, V>::end() {
    return Iterator(this, capacidad);
}

#endif // TABLA_HASH_TPP
