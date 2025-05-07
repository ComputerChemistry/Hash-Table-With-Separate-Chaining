# Demostración de Tabla Hash

Este proyecto es una demostración de una implementación de tabla hash en C++. Permite realizar operaciones básicas como insertar, buscar, eliminar elementos, y más. También incluye funcionalidades para medir el rendimiento de la tabla hash.

## Contenido

- `TablaHash.hpp`: Definición de la clase `TablaHash`, que incluye métodos para manejar colisiones y estadísticas.
- `TablaHash.tpp`: Implementaciones de la clase `TablaHash` (archivo de plantilla).
- `main.cpp`: Implementación de la demostración de la tabla hash.

**Nota:** El archivo `TablaHash.cpp` está vacío porque todas las implementaciones de la clase `TablaHash` se encuentran en el archivo de plantilla `TablaHash.tpp`.

## Clase `TablaHash`

La clase `TablaHash` es una plantilla que permite almacenar pares clave-valor. Soporta diferentes métodos de resolución de colisiones, incluyendo:

- **Encadenamiento**: Utiliza listas enlazadas para manejar colisiones.
- **Sondeo Lineal**: Busca la siguiente posición disponible en la tabla.
- **Sondeo Cuadrático**: Utiliza una función cuadrática para encontrar la siguiente posición.
- **Doble Hash**: Utiliza una segunda función hash para encontrar la siguiente posición.

### Métodos Principales

- `bool insertar(const K& clave, const V& valor)`: Inserta un nuevo par clave-valor en la tabla.
- `std::optional<V> buscar(const K& clave) const`: Busca un valor por su clave.
- `bool eliminar(const K& clave)`: Elimina un elemento de la tabla.
- `void mostrar() const`: Muestra todos los elementos de la tabla.
- `void mostrarEstadisticas() const`: Muestra estadísticas sobre inserciones, búsquedas y eliminaciones.
- `void cambiarMetodo(MetodoColision nuevoMetodo)`: Cambia el método de resolución de colisiones.
- `void configurarFactorCarga(float nuevoFactorMax, float nuevoFactorMin)`: Configura los factores de carga máximo y mínimo.
- `void cargarDatosPrueba(size_t cantidad)`: Carga datos de prueba en la tabla.
- `bool guardarEnArchivo(const std::string& nombreArchivo)`: Guarda la tabla en un archivo.
- `bool cargarDesdeArchivo(const std::string& nombreArchivo)`: Carga la tabla desde un archivo.

### Estructura de Datos

- **HashEntry**: Estructura que representa una entrada en la tabla hash, que incluye la clave, el valor, y un indicador de si está ocupada o eliminada.
- **Estadisticas**: Estructura que mantiene estadísticas sobre inserciones, búsquedas, eliminaciones, colisiones y rehashes.

### Iterador

La clase `TablaHash` también incluye un iterador que permite recorrer los elementos de la tabla de manera eficiente, ya sea utilizando encadenamiento o métodos de sondeo.

## Requisitos

- Compilador de C++ (C++11 o superior).
- Biblioteca estándar de C++.

## Instalación

1. Clona este repositorio:
   ```bash
   https://github.com/ComputerChemistry/Hash-Table-With-Separate-Chaining.git
