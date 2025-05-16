/**
 * \file TablaHash.hpp
 * Este archivo contiene la definición de la clase `TablaHash`, que implementa una tabla hash genérica
 * con métodos para insertar, buscar, eliminar elementos y manejar colisiones.
 * \author Joan Antonio Lazaro Silva
 * \date 06/05/2025
 */

#ifndef TABLA_HASH_HPP
#define TABLA_HASH_HPP

#include <vector>
#include <list>
#include <string>
#include <optional>
#include <utility>
#include <stdexcept>
#include <functional>
#include <sstream>
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>
#include <type_traits>

namespace myhashtable {

    enum MetodoColision {
        ENCADENAMIENTO,
        SONDEO_LINEAL,
        SONDEO_CUADRATICO,
        DOBLE_HASH
    };

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

        size_t elementos;
        std::vector<HashEntry> tabla;
        std::vector<std::list<std::pair<K, V>>> listas;
        size_t capacidad;
        size_t elementos_eliminados;
        float factorCargaMax;
        float factorCargaMin;
        MetodoColision metodo;

        struct Estadisticas {
            size_t inserciones;
            size_t busquedas;
            size_t eliminaciones;
            size_t colisiones;
            size_t rehashes;

            Estadisticas();
            void reset();
        };

        mutable Estadisticas stats;
        bool debugMode;

        static constexpr size_t CAPACIDAD_MINIMA = 10;
        static constexpr float FACTOR_MAX_MINIMO = 0.4f;
        static constexpr float FACTOR_MAX_MAXIMO = 0.95f;
        static constexpr float FACTOR_MIN_MINIMO = 0.1f;
        static constexpr float FACTOR_MIN_MAXIMO = 0.5f;

        template <typename T>
        static std::string any_to_string(const T& valor);

        template <typename T>
        T string_to_any(const std::string& str) const;

        float factorCarga() const;
        float factorCargaEfectivo() const;
        void rehash(size_t nuevaCapacidad);
        size_t encontrarSiguientePrimo(size_t n) const;
        bool esPrimo(size_t n) const;
        size_t funcionHash(const K& clave) const;
        size_t funcionHashSecundaria(const K& clave) const;

        class Iterator;
        friend class Iterator;

    public:
        size_t getElementos() const { return elementos; }
        size_t cantidadElementos() const { return elementos; }


        template <typename T>
        static std::string any_to_string_public(const T& valor);

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

        Iterator begin() const;
        Iterator end() const;

    };

    template <typename K, typename V>
    class TablaHash<K, V>::Iterator {
        friend class TablaHash<K, V>;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<K, V>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        Iterator(const TablaHash<K, V>* tabla, size_t inicio_indice);
        std::pair<K, V> operator*() const;
        Iterator& operator++();
        bool operator==(const Iterator& otro) const;
        bool operator!=(const Iterator& otro) const;

        size_t getIndice() const { return indice_actual; }

    private:
        const TablaHash<K, V>* tabla_ptr;
        size_t indice_actual;
        typename std::list<std::pair<K, V>>::iterator list_it;

        void encontrarSiguienteElemento();


    };



} // namespace myhashtable

#include "../Templates/TablaHash.tpp"

#endif // TABLA_HASH_HPP
