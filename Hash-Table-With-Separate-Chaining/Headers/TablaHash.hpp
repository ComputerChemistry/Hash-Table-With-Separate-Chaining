// TablaHash.hpp
#ifndef TABLA_HASH_HPP
#define TABLA_HASH_HPP

#include <vector>
#include <list>
#include <string>
#include <optional>
#include <utility>

/**
 * @brief Enum para los métodos de resolución de colisiones
 */
enum MetodoColision {
    ENCADENAMIENTO,    // Encadenamiento (listas enlazadas)
    SONDEO_LINEAL,     // Sondeo lineal
    SONDEO_CUADRATICO, // Sondeo cuadrático
    DOBLE_HASH         // Doble hashing
};

/**
 * @brief Clase template para una tabla hash genérica
 * @tparam K Tipo clave
 * @tparam V Tipo valor
 */
template <typename K, typename V>
class TablaHash {
private:
    struct HashEntry {
        K clave;
        V valor;
        bool ocupado;
        bool eliminado;

        explicit HashEntry();
        HashEntry(const K& k, const V& v);
    };

    struct Estadisticas {
        size_t inserciones;
        size_t busquedas;
        size_t eliminaciones;
        size_t colisiones;
        size_t rehashes;

        Estadisticas();
        void reset();
    };

    static constexpr size_t CAPACIDAD_MINIMA = 10;
    static constexpr float FACTOR_MAX_MINIMO = 0.4f;
    static constexpr float FACTOR_MAX_MAXIMO = 0.95f;
    static constexpr float FACTOR_MIN_MINIMO = 0.1f;
    static constexpr float FACTOR_MIN_MAXIMO = 0.5f;

    std::vector<HashEntry> tabla;                    // Para métodos abiertos
    std::vector<std::list<std::pair<K, V>>> listas;  // Para encadenamiento

    size_t capacidad;
    size_t elementos;
    float factorCargaMax;
    float factorCargaMin;
    MetodoColision metodo;
    mutable Estadisticas stats;
    bool debugMode;

    size_t funcionHash(const K& clave) const;
    size_t funcionHashSecundaria(const K& clave) const;
    float factorCarga() const;
    void rehash(size_t nuevaCapacidad);

    template <typename T>
    static std::string any_to_string(const T& valor);

    template <typename T>
    T string_to_any(const std::string& str) const;

public:
    explicit TablaHash(size_t capacidadInicial = 100,
              float factorMax = 0.7f,
              float factorMin = 0.3f,
              MetodoColision metodoColision = ENCADENAMIENTO);

    bool insertar(const K& clave, const V& valor);
    std::optional<V> buscar(const K& clave) const;
    bool eliminar(const K& clave);

    void mostrar() const;
    void mostrarEstadisticas() const;
    void mostrarDistribucion() const;
    void cambiarMetodo(MetodoColision nuevoMetodo);
    void configurarFactorCarga(float nuevoFactorMax, float nuevoFactorMin);
    void cargarDatosPrueba(size_t cantidad);
    void vaciar();
    bool guardarEnArchivo(const std::string& nombreArchivo);
    bool cargarDesdeArchivo(const std::string& nombreArchivo);
    void toggleDebugMode();
    std::string nombreMetodo() const;

    class Iterator {
    private:
        TablaHash<K, V>* tabla;
        size_t indice;
        typename std::list<std::pair<K, V>>::iterator listIt;

        void encontrarSiguienteElemento();

    public:
        Iterator(TablaHash<K, V>* t, size_t i);

        std::pair<K, V> operator*() const;
        Iterator& operator++();
        bool operator==(const Iterator& otro) const;
        bool operator!=(const Iterator& otro) const;
    };

    Iterator begin();
    Iterator end();
};

#include "TablaHash.tpp"

#endif // TABLA_HASH_HPP
