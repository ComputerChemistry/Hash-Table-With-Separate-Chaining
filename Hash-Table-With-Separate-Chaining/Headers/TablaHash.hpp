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

/** 
 * \brief Enum para los métodos de resolución de colisiones 
 */
enum MetodoColision {
    ENCADENAMIENTO,    ///< Encadenamiento (listas enlazadas)
    SONDEO_LINEAL,     ///< Sondeo lineal
    SONDEO_CUADRATICO, ///< Sondeo cuadrático
    DOBLE_HASH         ///< Doble hashing
};

/**  
 * \brief Clase template para una tabla hash genérica 
 * \tparam K Tipo clave 
 * \tparam V Tipo valor 
 */
template <typename K, typename V>
class TablaHash {
private:
    /** 
     * \brief Estructura que representa una entrada de la tabla hash 
     */
    struct HashEntry {
        K clave;        ///< Clave de la entrada
        V valor;       ///< Valor de la entrada
        bool ocupado;  ///< Indica si la entrada está ocupada
        bool eliminado; ///< Indica si la entrada ha sido eliminada

        /** \brief Constructor por defecto que inicializa la entrada como libre */
        explicit HashEntry();
        /** \brief Constructor que inicializa con clave y valor */
        HashEntry(const K& k, const V& v);
    };

    /** 
     * \brief Estructura que mantiene estadísticas de la tabla hash 
     */
    struct Estadisticas {
        size_t inserciones;   ///< Número de inserciones realizadas
        size_t busquedas;     ///< Número de búsquedas realizadas
        size_t eliminaciones;  ///< Número de eliminaciones realizadas
        size_t colisiones;    ///< Número de colisiones detectadas
        size_t rehashes;      ///< Número de operaciones de rehash realizadas

        /** \brief Constructor que inicializa todos los contadores a cero */
        Estadisticas();
        /** \brief Resetea todas las estadísticas a cero */
        void reset();
    };

    static constexpr size_t CAPACIDAD_MINIMA = 10; ///< Capacidad mínima permitida para la tabla
    static constexpr float FACTOR_MAX_MINIMO = 0.4f; ///< Factor máximo mínimo permitido
    static constexpr float FACTOR_MAX_MAXIMO = 0.95f; ///< Factor máximo máximo permitido
    static constexpr float FACTOR_MIN_MINIMO = 0.1f; ///< Factor mínimo mínimo permitido
    static constexpr float FACTOR_MIN_MAXIMO = 0.5f; ///< Factor mínimo máximo permitido

    std::vector<HashEntry> tabla;                    ///< Vector utilizado para métodos abiertos (sondeo)
    std::vector<std::list<std::pair<K, V>>> listas;  ///< Vector de listas usado para encadenamiento

    size_t capacidad; ///< Capacidad actual de la tabla
    size_t elementos; ///< Cantidad actual de elementos almacenados
    float factorCargaMax; ///< Factor de carga máximo permitido antes del rehash
    float factorCargaMin; ///< Factor de carga mínimo permitido para optimización
    MetodoColision metodo; ///< Método actual de resolución de colisiones
    mutable Estadisticas stats; ///< Estadísticas acumuladas
    bool debugMode; ///< Indica si el modo debug está activo

    /** 
     * \brief Función hash principal sobre la clave 
     * \param clave Clave a transformar
     * \return Índice hash correspondiente a la clave
     */
    size_t funcionHash(const K& clave) const;

    /** 
     * \brief Función hash secundaria para doble hash 
     * \param clave Clave a transformar
     * \return Índice hash secundario
     */
    size_t funcionHashSecundaria(const K& clave) const;

    /** 
     * \brief Calcula el factor de carga actual de la tabla 
     * \return Factor de carga (elementos / capacidad)
     */
    float factorCarga() const;

    /** 
     * \brief Realiza un rehash a una nueva capacidad 
     * \param nuevaCapacidad Nueva capacidad para la tabla
     */
    void rehash(size_t nuevaCapacidad);

    /**
     * \brief Convierte cualquier tipo a string 
     * \tparam T Tipo a convertir
     * \param valor Valor a convertir
     * \return Representación en string del valor
     */
    template <typename T>
    static std::string any_to_string(const T& valor);

    /**
     * \brief Convierte string a cualquier tipo T 
     * \tparam T Tipo destino
     * \param str String a convertir
     * \return Valor convertido al tipo T
     */
    template <typename T>
    T string_to_any(const std::string& str) const;

public:
    /**
     * \brief Constructor principal de la tabla hash
     * \param capacidadInicial Capacidad inicial de la tabla
     * \param factorMax Factor de carga máximo antes de rehash
     * \param factorMin Factor de carga mínimo
     * \param metodoColision Estrategia de resolución de colisiones
     */
    explicit TablaHash(size_t capacidadInicial = 100,
              float factorMax = 0.7f,
              float factorMin = 0.3f,
              MetodoColision metodoColision = ENCADENAMIENTO);

    /**
     * \brief Inserta un par clave-valor en la tabla
     * \param clave Clave del elemento
     * \param valor Valor asociado a la clave
     * \return true si la inserción fue exitosa, false si la clave ya existe
     */
    bool insertar(const K& clave, const V& valor);

    /**
     * \brief Busca un valor a partir de su clave
     * \param clave Clave a buscar
     * \return Valor encontrado envuelto en optional, o nullopt si no existe
     */
    std::optional<V> buscar(const K& clave) const;

    /**
     * \brief Elimina un elemento según la clave
     * \param clave Clave del elemento a eliminar
     * \return true si se eliminó correctamente, false si la clave no existe
     */
    bool eliminar(const K& clave);

    /**
     * \brief Muestra la tabla con sus elementos
     */
    void mostrar() const;

    /**
     * \brief Muestra estadísticas actuales de la tabla
     */
    void mostrarEstadisticas() const;

    /**
     * \brief Muestra la distribución de elementos en la tabla
     */
    void mostrarDistribucion() const;

    /**
     * \brief Cambia el método de resolución de colisiones
     * \param nuevoMetodo Nuevo método a usar
     */
    void cambiarMetodo(MetodoColision nuevoMetodo);

    /**
     * \brief Configura los factores de carga máximo y mínimo
     * \param nuevoFactorMax Nuevo factor máximo permitido
     * \param nuevoFactorMin Nuevo factor mínimo permitido
     */
    void configurarFactorCarga(float nuevoFactorMax, float nuevoFactorMin);

    /**
     * \brief Carga datos de prueba aleatorios en la tabla
     * \param cantidad Número de elementos de prueba a cargar
     */
    void cargarDatosPrueba(size_t cantidad);

    /**
     * \brief Vacía totalmente la tabla y limpia estadísitcas
     */
    void vaciar();

    /**
     * \brief Guarda la tabla en un archivo de texto
     * \param nombreArchivo Nombre del archivo destino
     * \return true si ocurrió éxito, false en caso contrario
     */
    bool guardarEnArchivo(const std::string& nombreArchivo);

    /**
     * \brief Carga la tabla desde un archivo de texto
     * \param nombreArchivo Nombre del archivo a cargar
     * \return true si ocurrió éxito, false en caso contrario
     */
    bool cargarDesdeArchivo(const std::string& nombreArchivo);

    /**
     * \brief Activa o desactiva el modo Debug para mostrar información adicional
     */
    void toggleDebugMode();

    /**
     * \brief Retorna el nombre del método de colisión actual
     * \return Nombre del método como string
     */
    std::string nombreMetodo() const;

    /**
     * \brief Clase iteradora para recorrer los elementos de la tabla hash
     */
    class Iterator {
    private:
        TablaHash<K, V>* tabla; ///< Puntero a la tabla
        size_t indice; ///< Índice actual de iteración
        typename std::list<std::pair<K, V>>::iterator listIt; ///< Iterador para listas (encadenamiento)

        void encontrarSiguienteElemento();

    public:
        Iterator(TablaHash<K, V>* t, size_t i);

        std::pair<K, V> operator*() const;
        Iterator& operator++();
        bool operator==(const Iterator& otro) const;
        bool operator!=(const Iterator& otro) const;
    };

    /**
     * \brief Retorna un iterador al inicio
     * \return Iterador apuntando al primer elemento válido
     */
    Iterator begin();

    /**
     * \brief Retorna un iterador al final
     * \return Iterador apuntando al final
     */
    Iterator end();
};

#include "TablaHash.tpp"

#endif // TABLA_HASH_HPP
